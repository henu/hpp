#ifndef HPP_CONNECTIONMANAGER_H
#define HPP_CONNECTIONMANAGER_H

#include "tcpconnection.h"
#include "condition.h"
#include "mutex.h"
#include "thread.h"

#include <vector>

namespace Hpp
{

class TCPConnection;

class Connectionmanager
{

	// Friends
	friend class TCPConnection;

public:

private:

	// Constructor and destructor
	Connectionmanager(void);
	~Connectionmanager(void);

	static void removerThread(void*);

	// Adds new TCPconnection to be destroyed
	static void addDestroyableTCPConnection(TCPConnection::RealConnection* conn);

	// Container for destroyable connections
	typedef std::vector< TCPConnection::RealConnection* > TCPConnections;

	// The only instance of this class
	static Connectionmanager instance;

	bool stop_requested;

	TCPConnections tcpconns_to_destroy;

	Mutex conns_mutex;
	Condition conns_cond;

	// Thread that removes connections
	Thread remover;

};

}

#endif

