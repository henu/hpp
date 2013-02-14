#include "tcpconnection.h"

#include "connectionmanager.h"
#include "exception.h"
#include "lock.h"

#include <errno.h>
#include <iostream>
#ifndef WIN32
#include <netdb.h>
#endif

namespace Hpp
{

TCPConnection::TCPConnection(void)
{
	rconn = new RealConnection;
	rconn->outbuffer_pending_lock = NULL;
	rconn->connected_state = CLOSED;
	rconn->host_or_ip = "";
	rconn->port = 0;
	rconn->lag_emulation = false;
}

TCPConnection::TCPConnection(std::string const& host_or_ip, uint16_t port)
{
	rconn = new RealConnection;
	rconn->outbuffer_pending_lock = NULL;
	rconn->connected_state = CLOSED;
	rconn->host_or_ip = "";
	rconn->port = 0;
	rconn->lag_emulation = false;

	connect(host_or_ip, port);
}

TCPConnection::~TCPConnection(void)
{
	HppAssert(rconn, "No RealConnect object!");

	// Just in case, ask close.
	close();

	Connectionmanager::addDestroyableTCPConnection(rconn);
	rconn = NULL;
}

void TCPConnection::connect(std::string const& host_or_ip, uint16_t port)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock connected_lock(rconn->connected_mutex);
	if (rconn->connected_state != CLOSED) {
		throw Exception("Already connected!");
	}

	#ifndef HPP_USE_SDL_NET

	// Get address from host
	hostent* address = gethostbyname(host_or_ip.c_str());
	if (address == NULL) {
		if (h_errno == HOST_NOT_FOUND) {
			throw Exception("Host \"" + host_or_ip + "\" does not exist!");
		} else if (h_errno == NO_ADDRESS || h_errno == NO_DATA) {
			throw Exception("Host \"" + host_or_ip + "\" does not have an IP-address!");
		} else if (h_errno == NO_RECOVERY) {
			throw HostUnavailable("Nameserver error while looking for host \"" + host_or_ip + "\"!");
		} else if (h_errno == TRY_AGAIN) {
			throw HostUnavailable("Temporary nameserver error while looking for host \"" + host_or_ip + "\"! Try again later.");
		} else {
			throw Exception("Unknown error occured while looking for host \"" + host_or_ip + "\"!");
		}
	}
	HppAssert(address->h_addr_list != NULL, "");

	// Clear socket address
	sockaddr_in soc_addr;
	soc_addr.sin_family = address->h_addrtype;
	soc_addr.sin_port = htons(port);
	soc_addr.sin_addr = *reinterpret_cast< in_addr* >(address->h_addr_list[0]);
	memset(&soc_addr.sin_zero, 0, 8);

	// Create new socket
	rconn->soc = socket(address->h_addrtype, SOCK_STREAM, 0);
	if (rconn->soc == -1) {
		throw Exception("Unable to create socket for host \"" + host_or_ip + "\"!");
	}

	if (::connect(rconn->soc, reinterpret_cast< struct sockaddr* >(&soc_addr), sizeof(struct sockaddr)) == -1) {
		shutdown(rconn->soc, SHUT_RDWR);
		::close(rconn->soc);
		throw HostUnavailable("Connection failed to \"" + host_or_ip + "\"!");
	}

	#else

	IPaddress sdl_ip;
	if (SDLNet_ResolveHost(&sdl_ip, host_or_ip.c_str(), port) == -1) {
		throw Exception(std::string("Unable to resolve host when trying to connect to host! Reason: ") + SDLNet_GetError());
	}

	rconn->sdlsoc = SDLNet_TCP_Open(&sdl_ip);
	if (!rconn->sdlsoc) {
		throw Exception(std::string("Unable to connect to host! Reason: ") + SDLNet_GetError());
	}
	#endif

	rconn->connected_state = CONNECTED;
	rconn->host_or_ip = host_or_ip;
	rconn->port = port;

	// Start reading and writing threads
	rconn->reader_thread = Thread(readerThread, reinterpret_cast< void* >(this));
	rconn->writer_thread = Thread(writerThread, reinterpret_cast< void* >(this));
	#ifndef NDEBUG
	rconn->reader_thread_id = rconn->reader_thread.getId();
	rconn->writer_thread_id = rconn->writer_thread.getId();
	#endif

}

uint16_t TCPConnection::getPort(void) const
{
	HppAssert(rconn, "No RealConnect object!");

// TODO: Protect with mutex!
	return rconn->port;
}

std::string TCPConnection::getHost(void) const
{
	HppAssert(rconn, "No RealConnect object!");

// TODO: Protect with mutex!
	return rconn->host_or_ip;
}

size_t TCPConnection::getAmountOfWaitingData(void)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock inbuffer_rcv_lock(rconn->inbuffer_rcv_mutex);
	size_t result = rconn->inbuffer_rcv.size();
	return result;
}

bool TCPConnection::waitForReading(size_t bytes)
{
	HppAssert(rconn, "No RealConnect object!");
	RealConnection* rconn2 = rconn;

	Lock inbuffer_rcv_lock(rconn2->inbuffer_rcv_mutex);
	while (rconn2->inbuffer_rcv.size() < bytes) {

		inbuffer_rcv_lock.unlock();

		// Check if there is no data to be copied to inbuffer_rcv.
		Lock reader_lock(rconn2->reader_mutex);

		// Ensure connection is not closed
		Lock connected_lock(rconn2->connected_mutex);
		if (rconn2->connected_state != CONNECTED) {
			waitUntilConnectionIsClosed(rconn2);
			return false;
		}
		connected_lock.unlock();

		if (rconn2->inbuffer.empty()) {
			rconn2->reader_cond.wait(rconn2->reader_mutex);
			// Ensure connection is not closed
			connected_lock.relock();
			if (rconn2->connected_state != CONNECTED) {
				waitUntilConnectionIsClosed(rconn2);
				return false;
			}
			connected_lock.unlock();
		}

		// If lag emulation is enabled, then wait
		// here until enough time has passed.
		size_t amount_to_copy = rconn2->inbuffer.size();
		if (rconn2->lag_emulation && !rconn2->inbuffer.empty()) {
			HppAssert(!rconn2->lag_emulation_queue.empty(), "Lag emulation queue out of sync!");

			Delay sleeping_left = rconn2->lag_emulation_queue.front().time - now();
			if (sleeping_left > Delay::secs(0)) {
				reader_lock.unlock();
				sleeping_left.sleep();
				reader_lock.relock();
			}
			amount_to_copy = rconn2->lag_emulation_queue.front().amount;
			rconn2->lag_emulation_queue.pop_front();
		}

		// Copy everything from inbuffer to inbuffer_rcv so we can
		// let reader work as long as possible without blocks. In
		// some situations, there are not really stuff in inbuffer,
		// but it does not matter.
		HppAssert(rconn2->inbuffer.size() >= amount_to_copy, "Too much to copy! There is not that much in the buffer!");
		inbuffer_rcv_lock.relock();
		for (size_t copy_ofs = 0; copy_ofs < amount_to_copy; ++ copy_ofs) {
			rconn2->inbuffer_rcv.push(rconn2->inbuffer.front());
			rconn2->inbuffer.pop();
		}

	}
	return true;
}

void TCPConnection::initWrite(void)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock* lock = new Lock(rconn->outbuffer_pending_mutex);
	HppAssert(!rconn->outbuffer_pending_lock, "Lock already exists!");
	rconn->outbuffer_pending_lock = lock;
}

void TCPConnection::deinitWrite(void)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock writer_lock(rconn->writer_mutex);
	for (ByteV::iterator outbuffer_pending_it = rconn->outbuffer_pending.begin();
	     outbuffer_pending_it != rconn->outbuffer_pending.end();
	     outbuffer_pending_it ++) {
		rconn->outbuffer.push(*outbuffer_pending_it);
	}
	rconn->outbuffer_pending.clear();
	writer_lock.unlock();
	rconn->writer_cond.signal();

	HppAssert(rconn->outbuffer_pending_lock, "Lock does not exist!");
	Lock* lock = rconn->outbuffer_pending_lock;
	rconn->outbuffer_pending_lock = NULL;
	delete lock;
}

void TCPConnection::enableLagEmulation(Delay const& lag)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock reader_lock(rconn->reader_mutex);
	rconn->lag_emulation = true;
	rconn->lag_emulation_amount = lag;
}

void TCPConnection::readerThread(void* conn_raw)
{
	// Initialize thread
	TCPConnection& conn = *reinterpret_cast< TCPConnection* >(conn_raw);
	RealConnection* rconn = conn.rconn;
	Mutex& reader_mutex = rconn->reader_mutex;
	Condition& reader_cond = rconn->reader_cond;
	ByteQ& inbuffer = rconn->inbuffer;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = rconn->soc;
	#else
	TCPsocket sdlsoc = rconn->sdlsoc;
	#endif
	bool& lag_emulation = rconn->lag_emulation;
	Delay& lag_emulation_amount = rconn->lag_emulation_amount;
	TimesAndAmounts& lag_emulation_queue = rconn->lag_emulation_queue;

	// Buffer where bytes are received into
	size_t const BUFFER_SIZE = 512;
	uint8_t buffer[BUFFER_SIZE];

	// Run thread
	#ifndef HPP_USE_SDL_NET
	bool got_bytes_last_time = false;
	do {

		// Wait atleast one
		ssize_t recv_bytes;
		if (got_bytes_last_time) {
			recv_bytes = recv(soc, (void*)buffer, BUFFER_SIZE, MSG_DONTWAIT);
		} else {
			recv_bytes = recv(soc, (void*)buffer, 1, MSG_WAITALL);
		}

		// Check if connection was closed
// TODO: This isn't closed by remote host, right?
		if (recv_bytes == 0) {
			closeByRemoteHost(rconn);
			return;
		}
		// If receiving was interrupted by a signal, then try again
		else if (recv_bytes < 0 && errno == EINTR) {
// TODO: Is it safe to just ignore signal?
		}
		// Check if no data was pending
		else if (recv_bytes < 0 && errno == EAGAIN) {
			got_bytes_last_time = false;
		}
		// Check if an error has occured
		else if (recv_bytes < 0) {
			// Check if connection was just closed
			if (errno == ECONNRESET || errno == ENOTCONN) {
				closeByRemoteHost(rconn);
				return;
			}
			throw Exception(std::string("Unable to receive data! Reason: ") + strerror(errno));
		}
		// Bytes were received. Add them to queue and notify
		// possible waiting thread. If lag emulation is enabled,
		// then also tell time when these bytes can be accessed.
		else {

			Lock reader_lock(reader_mutex);

			if (lag_emulation) {
				Time access_time = now() + lag_emulation_amount;
				lag_emulation_queue.push_back(TimeAndAmount(access_time, recv_bytes));
			}

			uint8_t* buffer_end = buffer + recv_bytes;
			for (uint8_t* buffer_it = buffer;
			     buffer_it != buffer_end;
			     buffer_it ++) {
				inbuffer.push(*buffer_it);
			}

			reader_lock.unlock();

			// Inform possible waiting thread
// TODO: Why this needs broadcast? We are supposed to have only one thread waiting for data!
			reader_cond.broadcast();

			got_bytes_last_time = true;
		}

	} while (true);
	#else
	do {
		ssize_t recv_bytes = SDLNet_TCP_Recv(sdlsoc, (void*)buffer, BUFFER_SIZE);
		if (recv_bytes == 0) {
			closeByRemoteHost(rconn);
			return;
		}
		// Check if an error has occured
		else if (recv_bytes < 0) {
			throw Exception(std::string("Unable to receive data! Reason: ") + SDLNet_GetError());
		}
		// Bytes were received. Add them to queue and notify possible
		// waiting thread.
		else {

			Lock reader_lock(reader_mutex);
			uint8_t* buffer_end = buffer + recv_bytes;
			for (uint8_t* buffer_it = buffer;
			     buffer_it != buffer_end;
			     buffer_it ++) {
				inbuffer.push(*buffer_it);
			}
			reader_lock.unlock();

			// Inform possible waiting thread
// TODO: Why this needs broadcast? We are supposed to have only one thread waiting for data!
			reader_cond.broadcast();

		}
// TODO: Check if connection is closed!
	} while (true);
	#endif

}

void TCPConnection::writerThread(void* conn_raw)
{
	// Initialize thread
	TCPConnection& conn = *reinterpret_cast< TCPConnection* >(conn_raw);
	RealConnection* rconn = conn.rconn;
	Mutex& writer_mutex = rconn->writer_mutex;
	Condition& writer_cond = rconn->writer_cond;
	Condition& writecheck_cond = rconn->writecheck_cond;
	ByteQ& outbuffer = rconn->outbuffer;
	State& connected_state = rconn->connected_state;
	Mutex& connected_mutex = rconn->connected_mutex;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = rconn->soc;
	#else
	TCPsocket sdlsoc = rconn->sdlsoc;
	#endif

	// Run thread
	ByteV outbuffer_v;
	do {

		Lock writer_lock(writer_mutex);

		// Ensure connection is not closed
		Lock connected_lock(connected_mutex);
		if (connected_state != CONNECTED) {
			writer_lock.unlock();
			waitUntilConnectionIsClosed(rconn);
			writecheck_cond.broadcast();
			return;
		}
		connected_lock.unlock();

		// If sending queue is empty wait for it to get stuff
		if (outbuffer.empty()) {
			writecheck_cond.broadcast();
			writer_cond.wait(writer_mutex);

			// Thread is being ran again. Check if connection is
			// closed.
			connected_lock.relock();
			if (connected_state != CONNECTED) {
				writer_lock.unlock();
				waitUntilConnectionIsClosed(rconn);
				writecheck_cond.broadcast();
				return;
			}
			connected_lock.unlock();

		}

		// Read queue to vector
		outbuffer_v.clear();
// TODO: This causes errors in send() when connection is closed! Is it our fault?
		outbuffer_v.reserve(outbuffer.size());
		while (!outbuffer.empty()) {
			outbuffer_v.push_back(outbuffer.front());
			outbuffer.pop();
		}
		HppAssert(outbuffer.empty(), "");
		writer_lock.unlock();

		// Send data
		#ifndef HPP_USE_SDL_NET
		ssize_t sent_bytes = ::send(soc, reinterpret_cast< const void* >(&outbuffer_v[0]), outbuffer_v.size(), MSG_NOSIGNAL);
		if (sent_bytes < static_cast< ssize_t >(outbuffer_v.size())) {
			// Check if remote host has closed connection
			if (errno == EPIPE ||
			    errno == ECONNRESET ||
			    errno == ENOTCONN) {
				closeByRemoteHost(rconn);
				writecheck_cond.broadcast();
				return;
			} else {
// TODO: This may occur when connection is properly closed!
				std::cerr << "WARNING: Unable to send data!" << std::endl;
				writecheck_cond.broadcast();
				return;
			}
		}
		#else
		ssize_t sent_bytes = SDLNet_TCP_Send(sdlsoc, reinterpret_cast< const void* >(&outbuffer_v[0]), outbuffer_v.size());
		if (sent_bytes < static_cast< ssize_t >(outbuffer_v.size())) {
// TODO: Errors are not checked! Is this bad?
			closeByRemoteHost(rconn);
			writecheck_cond.broadcast();
			return EXIT_SUCCESS;
		}
		#endif

	} while (true);

}

#ifndef HPP_USE_SDL_NET
TCPConnection::TCPConnection(int32_t soc, uint16_t port)
#else
TCPConnection::TCPConnection(TCPsocket sdlsoc, uint16_t port)
#endif
{
	rconn = new RealConnection;

	rconn->outbuffer_pending_lock = NULL;
	#ifndef HPP_USE_SDL_NET
	rconn->soc = soc;
	#else
	rconn->sdlsoc = sdlsoc;
	#endif
	rconn->port = port;
	rconn->lag_emulation = false;

	rconn->connected_state = CONNECTED;

	// Start reading and writing threads
	rconn->reader_thread = Thread(readerThread, reinterpret_cast< void* >(this));
	rconn->writer_thread = Thread(writerThread, reinterpret_cast< void* >(this));
	#ifndef NDEBUG
	rconn->reader_thread_id = rconn->reader_thread.getId();
	rconn->writer_thread_id = rconn->writer_thread.getId();
	#endif
}

void TCPConnection::cleanRealConnection(RealConnection* rconn)
{
	HppAssert(rconn, "No RealConnect object!");

	// Wait until all threads are closed.
	rconn->reader_thread.wait();
	rconn->writer_thread.wait();

	HppAssert(!rconn->outbuffer_pending_lock, "Lock is not opened!");

	// Mark mutexes destroyable
	rconn->reader_mutex.destroy();
	rconn->inbuffer_rcv_mutex.destroy();
	rconn->writer_mutex.destroy();
	rconn->outbuffer_pending_mutex.destroy();
	rconn->connected_mutex.destroy();

	delete rconn;
}

void TCPConnection::close(RealConnection* rconn, bool closed_by_remote_server)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock connected_lock(rconn->connected_mutex);
	if (rconn->connected_state == CLOSED) {
		return;
	}
	if (rconn->connected_state == CLOSING) {
		waitUntilConnectionIsClosed(rconn);
		return;
	}
	rconn->connected_state = CLOSING;
	connected_lock.unlock();

	// Signal possible waiting threads so they know to stop.
// TODO: Why this needs broadcast?
	rconn->reader_cond.broadcast();
	rconn->writer_cond.signal();

	waitUntilAllDataIsSent(rconn);

	// Clean connection. Allow writing of data.
	#ifndef HPP_USE_SDL_NET
	::shutdown(rconn->soc, SHUT_RD);
	::close(rconn->soc);
	#else
	SDLNet_TCP_Close(rconn->sdlsoc);
	#endif

	// Connection is now closed
	connected_lock.relock();
	rconn->connected_state = CLOSED;
	connected_lock.unlock();
	rconn->connected_cond.broadcast();
}

void TCPConnection::waitUntilAllDataIsSent(RealConnection* rconn)
{
	HppAssert(rconn, "No RealConnect object!");

	Lock writer_lock(rconn->writer_mutex);
	while (!rconn->outbuffer.empty()) {
		rconn->writecheck_cond.wait(rconn->writer_mutex);
	}
}

void TCPConnection::waitUntilConnectionIsClosed(RealConnection* rconn)
{
	HppAssert(rconn, "No RealConnect object!");

	if (rconn->connected_state == CLOSED) {
		return;
	}
	if (rconn->connected_state == CONNECTED) {
		throw Exception("Logical error! State of connection should be closed or closing!");
	}
	rconn->connected_cond.wait(rconn->connected_mutex);
	if (rconn->connected_state != CLOSED) {
		throw Exception("Logical error! State of connection should be closed now!");
	}
	return;
}

}

