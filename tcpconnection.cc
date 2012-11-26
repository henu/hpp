#include "tcpconnection.h"

#include "exception.h"
#include "lock.h"

#include <errno.h>
#include <iostream>
#ifndef WIN32
#include <netdb.h>
#endif

namespace Hpp
{

TCPConnection::TCPConnection(void) :
receiver(NULL),
receiver_data(NULL),
outbuffer_pending_lock(NULL),
connected(false),
host_or_ip(""),
port(0),
lag_emulation(false)
{
}

TCPConnection::TCPConnection(std::string const& host_or_ip, uint16_t port) :
receiver(NULL),
receiver_data(NULL),
outbuffer_pending_lock(NULL),
connected(false),
host_or_ip(""),
port(0),
lag_emulation(false)
{
	connect(host_or_ip, port);
}

TCPConnection::~TCPConnection(void)
{
	// Just in case, ask close.
	close();

	// Ensure destroying is not done from threads of this object.
	#ifndef NDEBUG
	HppAssert(getThisThreadID() != reader_thread_id, "TCPConnection destroyed in wrong thread!");
	HppAssert(getThisThreadID() != writer_thread_id, "TCPConnection destroyed in wrong thread!");
	HppAssert(getThisThreadID() != notifier_thread_id, "TCPConnection destroyed in wrong thread!");
	#endif

	// Wait until all threads are closed.
	reader_thread.wait();
	writer_thread.wait();
	notifier_thread.wait();

	// Mark mutexes destroyable
	receiver_mutex.destroy();
	reader_mutex.destroy();
	inbuffer_rcv_mutex.destroy();
	writer_mutex.destroy();
	outbuffer_pending_mutex.destroy();
	connected_mutex.destroy();

}

void TCPConnection::connect(std::string const& host_or_ip, uint16_t port)
{
	Lock connected_lock(connected_mutex);
	if (connected) {
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
	soc = socket(address->h_addrtype, SOCK_STREAM, 0);
	if (soc == -1) {
		throw Exception("Unable to create socket for host \"" + host_or_ip + "\"!");
	}

	if (::connect(soc, reinterpret_cast< struct sockaddr* >(&soc_addr), sizeof(struct sockaddr)) == -1) {
		shutdown(soc, SHUT_RDWR);
		::close(soc);
		throw HostUnavailable("Connection failed to \"" + host_or_ip + "\"!");
	}

	#else

	IPaddress sdl_ip;
	if (SDLNet_ResolveHost(&sdl_ip, host_or_ip.c_str(), port) == -1) {
		throw Exception(std::string("Unable to resolve host when trying to connect to host! Reason: ") + SDLNet_GetError());
	}

	sdlsoc = SDLNet_TCP_Open(&sdl_ip);
	if (!sdlsoc) {
		throw Exception(std::string("Unable to connect to host! Reason: ") + SDLNet_GetError());
	}
	#endif

	connected = true;
	this->host_or_ip = host_or_ip;
	this->port = port;

	// Start reading, writing and notifier threads
	reader_thread = Thread(readerThread, reinterpret_cast< void* >(this));
	writer_thread = Thread(writerThread, reinterpret_cast< void* >(this));
	notifier_thread = Thread(notifierThread, reinterpret_cast< void* >(this));
	#ifndef NDEBUG
	reader_thread_id = reader_thread.getId();
	writer_thread_id = writer_thread.getId();
	notifier_thread_id = notifier_thread.getId();
	#endif

}

void TCPConnection::setDataReceiver(DataReceiver receiver, void* data)
{
	Lock receiver_lock(receiver_mutex);
	this->receiver = receiver;
	receiver_data = data;
	receiver_lock.unlock();
	reader_cond.broadcast();
}

uint16_t TCPConnection::getPort(void) const
{
// TODO: Protect with mutex!
	return port;
}

std::string TCPConnection::getHost(void) const
{
// TODO: Protect with mutex!
	return host_or_ip;
}

size_t TCPConnection::getAmountOfWaitingData(void)
{
	Lock inbuffer_rcv_lock(inbuffer_rcv_mutex);
	size_t result = inbuffer_rcv.size();
	return result;
}

bool TCPConnection::waitForReading(size_t bytes)
{
	Lock inbuffer_rcv_lock(inbuffer_rcv_mutex);
	while (inbuffer_rcv.size() < bytes) {

		inbuffer_rcv_lock.unlock();

		// Check if there is no data to be copied to inbuffer_rcv.
		Lock reader_lock(reader_mutex);

		// Ensure connection is not closed
		Lock connected_lock(connected_mutex);
		if (!connected) {
			return false;
		}
		connected_lock.unlock();

		if (inbuffer.empty()) {
			reader_cond.wait(reader_mutex);
			// Ensure connection is not closed
			connected_lock.relock();
			if (!connected) {
				return false;
			}
			connected_lock.unlock();
		}

		// If lag emulation is enabled, then wait
		// here until enough time has passed.
		size_t amount_to_copy = inbuffer.size();
		if (lag_emulation && !inbuffer.empty()) {
			ssize_t bytes_to_check = bytes;
			HppAssert(!lag_emulation_queue.empty(), "Lag emulation queue out of sync!");

			Delay sleeping_left = lag_emulation_queue.front().time - now();
			if (sleeping_left > Delay::secs(0)) {
				reader_lock.unlock();
				sleeping_left.sleep();
				reader_lock.relock();
			}
			amount_to_copy = lag_emulation_queue.front().amount;
			lag_emulation_queue.pop_front();
		}

		// Copy everything from inbuffer to inbuffer_rcv so we can let
		// receiver thread work as long as possible without blocks. In
		// some situations, there are not really stuff in inbuffer, but
		// it does not matter.
		HppAssert(inbuffer.size() >= amount_to_copy, "Too much to copy! There is not that much in the buffer!");
		inbuffer_rcv_lock.relock();
		for (size_t copy_ofs = 0; copy_ofs < amount_to_copy; ++ copy_ofs) {
			inbuffer_rcv.push(inbuffer.front());
			inbuffer.pop();
		}

	}
	return true;
}

void TCPConnection::initWrite(void)
{
	Lock* lock = new Lock(outbuffer_pending_mutex);
	HppAssert(!outbuffer_pending_lock, "Lock already exists!");
	outbuffer_pending_lock = lock;
}

void TCPConnection::deinitWrite(void)
{
	Lock writer_lock(writer_mutex);
	for (ByteV::iterator outbuffer_pending_it = outbuffer_pending.begin();
	     outbuffer_pending_it != outbuffer_pending.end();
	     outbuffer_pending_it ++) {
		outbuffer.push(*outbuffer_pending_it);
	}
	outbuffer_pending.clear();
	writer_lock.unlock();
	writer_cond.signal();

	HppAssert(outbuffer_pending_lock, "Lock does not exist!");
	Lock* lock = outbuffer_pending_lock;
	outbuffer_pending_lock = NULL;
	delete lock;
}

void TCPConnection::waitUntilAllDataIsSent(void)
{
	Lock writer_lock(writer_mutex);
	while (!outbuffer.empty()) {
		writecheck_cond.wait(writer_mutex);
	}
}

void TCPConnection::enableLagEmulation(Delay const& lag)
{
	Lock reader_lock(reader_mutex);
	lag_emulation = true;
	lag_emulation_amount = lag;
}

void TCPConnection::readerThread(void* conn_raw)
{

	// Initialize thread
	TCPConnection& conn = *reinterpret_cast< TCPConnection* >(conn_raw);
	Mutex& reader_mutex = conn.reader_mutex;
	Condition& reader_cond = conn.reader_cond;
	ByteQ& inbuffer = conn.inbuffer;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = conn.soc;
	#else
	TCPsocket sdlsoc = conn.sdlsoc;
	#endif
	bool& lag_emulation = conn.lag_emulation;
	Delay& lag_emulation_amount = conn.lag_emulation_amount;
	TimesAndAmounts& lag_emulation_queue = conn.lag_emulation_queue;

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
			conn.closeByRemoteHost();
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
				conn.closeByRemoteHost();
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
			conn.closeByRemoteHost();
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
	Mutex& writer_mutex = conn.writer_mutex;
	Condition& writer_cond = conn.writer_cond;
	Condition& writecheck_cond = conn.writecheck_cond;
	ByteQ& outbuffer = conn.outbuffer;
	bool& connected = conn.connected;
	Mutex& connected_mutex = conn.connected_mutex;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = conn.soc;
	#else
	TCPsocket sdlsoc = conn.sdlsoc;
	#endif

	// Run thread
	ByteV outbuffer_v;
	do {

		Lock writer_lock(writer_mutex);

		// Ensure connection is not closed
		Lock connected_lock(connected_mutex);
		if (!connected) {
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
			if (!connected) {
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
				conn.closeByRemoteHost();
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
			conn.closeByRemoteHost();
			writecheck_cond.broadcast();
			return EXIT_SUCCESS;
		}
		#endif

	} while (true);

}

void TCPConnection::notifierThread(void* conn_raw)
{

	// Initialize thread
	TCPConnection& conn = *reinterpret_cast< TCPConnection* >(conn_raw);
	Mutex& reader_mutex = conn.reader_mutex;
	Condition& reader_cond = conn.reader_cond;
	ByteQ& inbuffer = conn.inbuffer;
	ByteQ& inbuffer_rcv = conn.inbuffer_rcv;
	Mutex& inbuffer_rcv_mutex = conn.inbuffer_rcv_mutex;
	bool& connected = conn.connected;
	Mutex& connected_mutex = conn.connected_mutex;
	DataReceiver& receiver = conn.receiver;
	void*& receiver_data = conn.receiver_data;
	Mutex& receiver_mutex = conn.receiver_mutex;

	// Run thread
	do {

		Lock reader_lock(reader_mutex);

		// Ensure connection is not closed
		Lock connected_lock(connected_mutex);
		if (!connected) {
			return;
		}
		connected_lock.unlock();

		// Check if there is received data available
		Lock inbuffer_rcv_lock(inbuffer_rcv_mutex);
		if (inbuffer.empty() && inbuffer_rcv.empty()) {
			inbuffer_rcv_lock.unlock();
			reader_cond.wait(reader_mutex);
			// Check if connection was closed
			connected_lock.relock();
			if (!connected) {
				return;
			}
			connected_lock.unlock();
		} else {
			inbuffer_rcv_lock.unlock();
		}

		reader_lock.unlock();

		// Notify datareceiver
		Lock receiver_lock(receiver_mutex);
		if (receiver) {
			receiver(&conn, receiver_data);
		}
		// If there was no receiver set, then wait for it to appear set
		else {
			reader_cond.wait(receiver_mutex);
			// Check if connection was closed
			connected_lock.relock();
			if (!connected) {
				return;
			}
			connected_lock.unlock();
		}

	} while (true);

}

#ifndef HPP_USE_SDL_NET
TCPConnection::TCPConnection(int32_t soc, uint16_t port) :
#else
TCPConnection::TCPConnection(TCPsocket sdlsoc, uint16_t port) :
#endif
receiver(NULL),
outbuffer_pending_lock(NULL),
#ifndef HPP_USE_SDL_NET
soc(soc),
#else
sdlsoc(sdlsoc),
#endif
port(port),
lag_emulation(false)
{

	connected = true;

	// Start reading, writing and notifier threads
	reader_thread = Thread(readerThread, reinterpret_cast< void* >(this));
	writer_thread = Thread(writerThread, reinterpret_cast< void* >(this));
	notifier_thread = Thread(notifierThread, reinterpret_cast< void* >(this));
	#ifndef NDEBUG
	reader_thread_id = reader_thread.getId();
	writer_thread_id = writer_thread.getId();
	notifier_thread_id = notifier_thread.getId();
	#endif
}

void TCPConnection::close(bool closed_by_remote_server)
{

	// If connection is already closed, do nothing. Mark connection closed
	// if it is still open
	Lock connected_lock(connected_mutex);
	if (!connected) {
		return;
	}
	connected_lock.unlock();

	// Lock reader and writer threads so we can ensure that they are in
	// proper state when connection is marked as closed.
	Lock reader_lock(reader_mutex);
	Lock writer_lock(writer_mutex);
	connected_lock.relock();
	connected = false;
	reader_lock.unlock();
	writer_lock.unlock();
	connected_lock.unlock();

	// Signal possible waiting reader threads so they knows to stop.
// TODO: Why this needs broadcast?
	reader_cond.broadcast();
	writer_cond.signal();

	// Clean connection. Allow writing of data.
	#ifndef HPP_USE_SDL_NET
	::shutdown(soc, SHUT_RD);
	::close(soc);
	#else
	SDLNet_TCP_Close(sdlsoc);
	#endif

	// Inform possible data receiver about closed connection.
	// Do this only when connection is closed by remote host.
	if (closed_by_remote_server) {
		Lock receiver_lock(receiver_mutex);
		if (receiver) {
			receiver(this, receiver_data);
		}
	}
}

}

