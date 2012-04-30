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

TCPConnection::TCPConnection(std::string const& host_or_ip, uint16_t port) :
destroyable(false),
receiver(NULL),
receiver_data(NULL),
outbuffer_pending_lock(NULL),
host_or_ip(host_or_ip),
port(port)
{

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

	if (connect(soc, reinterpret_cast< struct sockaddr* >(&soc_addr), sizeof(struct sockaddr)) == -1) {
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
	if(!sdlsoc) {
		throw Exception(std::string("Unable to connect to host! Reason: ") + SDLNet_GetError());
	}
	#endif

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

	Connectionmanager::registerTCPConnection(this);

}

void TCPConnection::destroy(void)
{
	Lock destroyable_lock(destroyable_mutex);
	if (!destroyable) {
		destroyable = true;
		Connectionmanager::destroyTCPConnection(this);
	}
}

void TCPConnection::setDataReceiver(DataReceiver receiver, void* data)
{
	Lock receiver_lock(receiver_mutex);
	this->receiver = receiver;
	receiver_data = data;
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

		// Copy everything from inbuffer to inbuffer_rcv so we can let
		// receiver thread work as long as possible without blocks. In
		// some situations, there are not really stuff in inbuffer, but
		// it does not matter.
		inbuffer_rcv_lock.relock();
		while (!inbuffer.empty()) {
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
	DataReceiver receiver = conn.receiver;
	void* receiver_data = conn.receiver_data;
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

	} while (true);

}

#ifndef HPP_USE_SDL_NET
TCPConnection::TCPConnection(int32_t soc, uint16_t port) :
#else
TCPConnection::TCPConnection(TCPsocket sdlsoc, uint16_t port) :
#endif
destroyable(false),
receiver(NULL),
outbuffer_pending_lock(NULL),
#ifndef HPP_USE_SDL_NET
soc(soc),
#else
sdlsoc(sdlsoc),
#endif
port(port)
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

	Connectionmanager::registerTCPConnection(this);
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
	destroyable_mutex.destroy();
	receiver_mutex.destroy();
	reader_mutex.destroy();
	inbuffer_rcv_mutex.destroy();
	writer_mutex.destroy();
	outbuffer_pending_mutex.destroy();
	connected_mutex.destroy();

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
	Lock destroyable_lock(destroyable_mutex);
	if (!destroyable) {
		destroyable_lock.unlock();
		Lock receiver_lock(receiver_mutex);
		if (receiver) {
			receiver(this, receiver_data);
		}
	}
}

}

