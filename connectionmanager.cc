#include "connectionmanager.h"

#include "tcpconnection.h"
#include "lock.h"

#include <algorithm>
#include <iostream>
#ifdef HPP_USE_SDL_NET
#include "SDL_net.h"
#endif


namespace Hpp
{

// Static members
Connectionmanager Connectionmanager::instance;

Connectionmanager::Connectionmanager(void) :
stop_requested(false)
{
	// Init SDL_net if it's being used
	#ifdef HPP_USE_SDL_NET
	if (SDLNet_Init() == -1) {
		std::cerr << "WARNING: Unable to initialize SDL_net! Reason: " << SDL_GetError() << std::endl;
	}
	#endif
	// Start remover thread
	remover = Thread(removerThread, NULL);
}

Connectionmanager::~Connectionmanager(void)
{
	Lock conns_lock(conns_mutex);
	stop_requested = true;
	conns_lock.unlock();
	conns_cond.signal();
	try {
		remover.wait();
	}
	catch (Hpp::Exception const& e) {
		std::cerr << "ERROR: Connectionmanager has failed: " << e.what() << std::endl;
	}
	#ifdef HPP_USE_SDL_NET
	SDLNet_Quit();
	#endif
}

void Connectionmanager::removerThread(void*)
{
	do {

		Lock conns_lock(instance.conns_mutex);

		if (instance.stop_requested) {
			break;
		}

		if (instance.tcpconns_to_destroy.empty()) {
			instance.conns_cond.wait(instance.conns_mutex);
			if (instance.stop_requested) {
				break;
			}
		}

		for (TCPConnections::iterator tcpconns_to_destroy_it = instance.tcpconns_to_destroy.begin();
		     tcpconns_to_destroy_it != instance.tcpconns_to_destroy.end();
		     tcpconns_to_destroy_it ++) {
			try {
				TCPConnection::cleanRealConnection(*tcpconns_to_destroy_it);
			}
			catch (Hpp::Exception const& e) {
				throw Hpp::Exception("Unable to clean connection! Reason: " + std::string(e.what()));
			}
		}
		instance.tcpconns_to_destroy.clear();

	} while (true);
	return;
}

void Connectionmanager::addDestroyableTCPConnection(TCPConnection::RealConnection* conn)
{
	Lock conns_lock(instance.conns_mutex);
	HppAssert(std::find(instance.tcpconns_to_destroy.begin(), instance.tcpconns_to_destroy.end(), conn) == instance.tcpconns_to_destroy.end(), "Connection already registered!");
	instance.tcpconns_to_destroy.push_back(conn);
	conns_lock.unlock();
	instance.conns_cond.signal();
}

}

