#ifndef HPP_COMMANDEXEC_H
#define HPP_COMMANDEXEC_H

#ifndef WIN32

#include "ostreampipe.h"
#include "istreampipe.h"
#include "streambuf.h"
#include "thread.h"

#include <stdexcept>
#include <sys/types.h>

namespace Hpp
{

struct StreamfillerInfo;

class CommandExec
{

public:

	typedef uint8_t Flags;
	static Flags const STATELESS_OUT_STREAM	= 0x01;
	static Flags const STATELESS_ERR_STREAM	= 0x02;
	static Flags const STATELESS_IN_STREAM	= 0x04;
	static Flags const STATELESS_STREAMS	= 0x07;

	CommandExec(std::string const& command, Flags flags = 0);
	~CommandExec(void);

	// Waits until command has finished.
// TODO: Returns what?
	int wait(void);

	// Forces ending of process
	void kill(void);

	// Stream pipes for reading output/error and for giving input
	IStreampipe out;
	IStreampipe err;
/*
	OStreampipe in;
*/

private:

	Flags flags;

	StreamfillerInfo* out_sfinfo;
	StreamfillerInfo* err_sfinfo;
	StreamfillerInfo* in_sfinfo;

	pid_t child_pid;
	bool child_quit;

	int child_outd;
	int child_errd;
	int child_ind;

	Streambuf out_sbuf;
	Streambuf err_sbuf;
	Streambuf in_sbuf;

	int child_return;

	Thread out_writer;
	Thread err_writer;

	bool stop_threads;

};

}

#endif

#endif

