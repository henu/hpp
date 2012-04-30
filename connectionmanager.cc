#include "connectionmanager.h"

#include "tcpconnection.h"
#include "lock.h"

#include <iostream>
#ifdef HPP_USE_SDL_NET
#include "SDL_net.h"
#endif


namespace Hpp
{

// Static members
Connectionmanager Connectionmanager::instance;

void Connectionmanager::removerThread(void*)
{
	do {

		Lock conns_lock(instance.conns_mutex);

		if (instance.stop_requested) {
			break;
		}

		if (instance.tcpconns_removable.empty()) {
			instance.conns_cond.wait(instance.conns_mutex);
			if (instance.stop_requested) {
				break;
			}
		}

		for (TCPConnections::iterator tcpconns_removable_it = instance.tcpconns_removable.begin();
		     tcpconns_removable_it != instance.tcpconns_removable.end();
		     tcpconns_removable_it ++) {
			delete *tcpconns_removable_it;
		}
		instance.tcpconns_removable.clear();

	} while (true);
	return;
}

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
	remover.wait();
	// Destroy remaining connections
	conns_lock.relock();
	if (!tcpconns.empty()) {
		std::cerr << "WARNING: Some TCP connection objects were not destroyed!" << std::endl;
		for (TCPConnections::iterator tcpconns_it = tcpconns.begin();
		     tcpconns_it != tcpconns.end();
		     tcpconns_it ++) {
			delete *tcpconns_it;
		}
	}
	conns_lock.unlock();
	#ifdef HPP_USE_SDL_NET
	SDLNet_Quit();
	#endif
}

void Connectionmanager::registerTCPConnection(TCPConnection* conn)
{
	Lock conns_lock(instance.conns_mutex);
	HppAssert(instance.tcpconns.find(conn) == instance.tcpconns.end(), "Connection already registered!");
	instance.tcpconns.insert(conn);
}

void Connectionmanager::destroyTCPConnection(TCPConnection* conn)
{
	Lock conns_lock(instance.conns_mutex);
	HppAssert(instance.tcpconns_removable.find(conn) == instance.tcpconns_removable.end(), "Connection is already being marked as destroyable!");
	HppAssert(instance.tcpconns.find(conn) != instance.tcpconns.end(), "Connection does not exists!");
	instance.tcpconns_removable.insert(conn);
	instance.tcpconns.erase(conn);
	conns_lock.unlock();
	instance.conns_cond.signal();
}

}

