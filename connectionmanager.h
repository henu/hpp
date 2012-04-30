#ifndef HPP_CONNECTIONMANAGER_H
#define HPP_CONNECTIONMANAGER_H

#include "condition.h"
#include "mutex.h"
#include "thread.h"

#include <set>

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

	// Registers new connection to manager
	static void registerTCPConnection(TCPConnection* conn);

	// Asks for destruction to specific connection
	static void destroyTCPConnection(TCPConnection* conn);

	// Container for connections
	typedef std::set< TCPConnection* > TCPConnections;


	// The only instance of this class
	static Connectionmanager instance;

	bool stop_requested;

	TCPConnections tcpconns;
	TCPConnections tcpconns_removable;

	Mutex conns_mutex;
	Condition conns_cond;

	// Thread that removes connections
	Thread remover;


};

}

#endif

