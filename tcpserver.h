#ifndef HPP_TCPSERVER_H
#define HPP_TCPSERVER_H

#ifdef WIN32
#ifndef HPP_USE_SDL_NET
#define HPP_USE_SDL_NET
#endif
#endif

#include "thread.h"

#include <map>
#include <stdint.h>
#ifdef HPP_USE_SDL_NET
#include <SDL/SDL_net.h>
#endif

namespace Hpp
{

class TCPConnection;

class TCPServer
{

public:

	typedef void (*ConnHandlerFunc)(TCPConnection* new_conn, uint16_t port, void*);

	~TCPServer(void);

	// Starts/stop listening for new connections. New
	// TCPConnections must be destroyed by the user.
	void startListening(uint16_t port, ConnHandlerFunc connhandler, void* data);
	void stopListening(uint16_t port);

	bool isListeningPort(uint16_t port) const;

private:

	// Struct for providing information to listener threads
	struct ListenerInfo
	{
		ConnHandlerFunc connhandler;
		void* connhandler_data;
		#ifndef HPP_USE_SDL_NET
		int32_t soc;
		#else
		TCPsocket sdlsoc;
		#endif
		uint16_t port;
	};

	// Listener of one port
	struct Listener
	{
		Thread thread;
		#ifndef HPP_USE_SDL_NET
		int32_t soc;
		#else
		TCPsocket sdlsoc;
		#endif
	};
	typedef std::map< uint16_t, Listener > Listeners;

	// Listeners indexed by ports they listen
	Listeners listeners;

	static void listenerThread(void* linfo_raw);
};

}

#endif

