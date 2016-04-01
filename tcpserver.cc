#include "tcpserver.h"

#include "tcpconnection.h"
#include "cast.h"
#include "assert.h"

#include <strings.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif

namespace Hpp
{

TCPServer::~TCPServer(void)
{
	// Go listeners trough and stop them
	while (!listeners.empty()) {
		uint16_t port = listeners.begin()->first;
		stopListening(port);
	}
}

void TCPServer::startListening(uint16_t port, ConnHandlerFunc connhandler, void* data)
{
	// Ensure listener for this port does not already exist
	if (listeners.find(port) != listeners.end()) {
		throw Exception("Port " + sizeToStr(port) + " is already being listened!");
	}

	// Create new socket for listening incoming connections
	#ifndef HPP_USE_SDL_NET
	int32_t new_soc = socket(PF_INET, SOCK_STREAM, 0);
	if (new_soc == -1) {
		throw Exception("Unable to create socket for listening port " + sizeToStr(port) + "!");
	}

	// Clear socket address
	sockaddr_in soc_addr;
	bzero(&soc_addr, sizeof(soc_addr));

	soc_addr.sin_family = AF_INET;
	soc_addr.sin_port = htons(port);
	soc_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket
	if (bind(new_soc, reinterpret_cast< sockaddr* >(&soc_addr), sizeof(soc_addr)) == -1) {
		shutdown(new_soc, SHUT_RDWR);
		::close(new_soc);
		throw Exception("Unable to bind socket for listening port " + sizeToStr(port) + "! Reason: " + std::string(strerror(errno)));
	}

	// Start listening for connections
	if (listen(new_soc, 16) == -1) {
		shutdown(new_soc, SHUT_RDWR);
		::close(new_soc);
		throw Exception("Unable to listen socket in port " + sizeToStr(port) + "!");
	}

	// Spawn new thread to listen the given port
	ListenerInfo* li = new ListenerInfo;
	li->connhandler = connhandler;
	li->connhandler_data = data;
	li->soc = new_soc;
	li->port = port;
	Listener lst;
	lst.thread = Thread(listenerThread, reinterpret_cast< void* >(li));
	lst.soc = new_soc;
	#else
	IPaddress sdl_ip;

	// "Resolve host"
	if (SDLNet_ResolveHost(&sdl_ip, NULL, port) == -1) {
		throw Exception("Unable to resolve host when trying to listen socket in port " + sizeToStr(port) + "! Reason: " + SDLNet_GetError());
	}

	// Start listening for connections
	TCPsocket new_sdlsoc = SDLNet_TCP_Open(&sdl_ip);
	if (!new_sdlsoc) {
		throw Exception("Unable to listen socket in port " + sizeToStr(port) + "! Reason: " + SDLNet_GetError());
	}
	// Spawn new thread to listen the given port
	ListenerInfo* li = new ListenerInfo;
	li->connhandler = connhandler;
	li->connhandler_data = data;
	li->sdlsoc = new_sdlsoc;
	li->port = port;
	Listener lst;
	lst.thread = Thread(listenerThread, reinterpret_cast< void* >(li));
	lst.sdlsoc = new_sdlsoc;
	#endif

	HppAssert(listeners.find(port) == listeners.end(), "This port is already being listened!");
	listeners[port] = lst;

}

void TCPServer::stopListening(uint16_t port)
{
	// Ensure listener for this port does exist
	if (listeners.find(port) == listeners.end()) {
		throw Exception("Port " + sizeToStr(port) + " is not being listened!");
	}

	// Get thread and socket and then make container smaller
	Thread thread = listeners[port].thread;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = listeners[port].soc;
	#else
	TCPsocket sdlsoc = listeners[port].sdlsoc;
	#endif
	listeners.erase(port);

	// Close socket
	#ifndef HPP_USE_SDL_NET
	shutdown(soc, SHUT_RDWR);
	::close(soc);
	#else
	SDLNet_TCP_Close(sdlsoc);
	#endif

	// Wait for thread to finish
	thread.wait();
}

bool TCPServer::isListeningPort(uint16_t port) const
{
	return listeners.find(port) != listeners.end();
}

void TCPServer::listenerThread(void* linfo_raw)
{

	ListenerInfo* linfo = reinterpret_cast< ListenerInfo* >(linfo_raw);
	ConnHandlerFunc connhandler = linfo->connhandler;
	void* connhandler_data = linfo->connhandler_data;
	#ifndef HPP_USE_SDL_NET
	int32_t soc = linfo->soc;
	#else
	TCPsocket sdlsoc = linfo->sdlsoc;
	#endif
	uint16_t port = linfo->port;

	// Wait for new connections
	do {

		// Get new connected socket
		#ifndef HPP_USE_SDL_NET
		int32_t csoc = accept(soc, NULL, NULL);
// TODO: Is closed connection noticed here? If yes, then do something!
		if (csoc < 0) {
			// Check if connection is closed
			if (errno == EINVAL) {
				return;
			}
			shutdown(soc, SHUT_RDWR);
			::close(soc);
			throw Exception(std::string("Unable to accept incoming connection! Reason: ") + strerror(errno));
		}

		// Create new connection object
		TCPConnection* newconn = new TCPConnection(csoc, port);
		#else
		TCPsocket csoc;
		do {
			csoc = SDLNet_TCP_Accept(sdlsoc);
			if (csoc) {
				break;
			}
// TODO: Check if connection is closed!
			SDL_Delay(500);
		} while (true);

		// Create new connection object
		TCPConnection* newconn = new TCPConnection(csoc, port);
		#endif

		// Notify subclass
		connhandler(newconn, port, connhandler_data);

	} while (true);

}

}

