#include "commandexec.h"

#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

namespace Hpp
{

struct StreamfillerInfo
{
	int fd;
	Streambuf* sbuf;
	bool* stop;
};

size_t streamWriter(uint8_t* buf, size_t buf_size, void* sfinfo_raw);

void bufWriter(void* sfinfo_raw);

CommandExec::CommandExec(std::string const& command, Flags flags) :
out(&out_sbuf),
err(&err_sbuf),
flags(flags),
child_quit(false),
stop_threads(false)
{

	out_sfinfo = new StreamfillerInfo;
	err_sfinfo = new StreamfillerInfo;
	in_sfinfo = new StreamfillerInfo;

	int outpipe[2];
	int inpipe[2];
	int errpipe[2];

	(void)::pipe(outpipe);
	(void)::pipe(inpipe);
	(void)::pipe(errpipe);

	int stdin_old = ::dup(STDIN_FILENO);
	int stdout_old = ::dup(STDOUT_FILENO);
	int stderr_old = ::dup(STDERR_FILENO);

	::close(STDIN_FILENO);
	::close(STDOUT_FILENO);
	::close(STDERR_FILENO);

	::dup2(inpipe[1], STDOUT_FILENO);
	::dup2(outpipe[0], STDIN_FILENO);
	::dup2(errpipe[0], STDERR_FILENO);

	child_pid = ::fork();
	if (child_pid == 0) {

		// Close default io descriptors
		::close(STDOUT_FILENO);
		::close(STDIN_FILENO);
		::close(STDERR_FILENO);

		// Use created pipes as io descriptors
		::dup2(outpipe[0], STDIN_FILENO);
		::dup2(inpipe[1], STDOUT_FILENO);
		::dup2(errpipe[1], STDERR_FILENO);

		// Close descriptors. They are not needed anymore in this process.
		::close(outpipe[0]);
		::close(outpipe[1]);
		::close(inpipe[0]);
		::close(inpipe[1]);
		::close(errpipe[0]);
		::close(errpipe[1]);

		// Execute the command
		child_return = ::system(command.c_str());

		::exit(0);
	} else if (child_pid > 0) {

		// Restore normal descriptors.
		::close(STDOUT_FILENO);
		::close(STDIN_FILENO);
		::close(STDERR_FILENO);
		::dup2(stdout_old, STDOUT_FILENO);
		::dup2(stdin_old, STDIN_FILENO);
		::dup2(stderr_old, STDERR_FILENO);

		// These are not needed in this process.
		::close(outpipe[0]);
		::close(inpipe[1]);
		::close(errpipe[1]);

		child_ind = outpipe[1];
		child_outd = inpipe[0];
		child_errd = errpipe[0];

	} else {
		throw std::runtime_error("Unable to fork!");
	}

	// Handler of out stream
	out_sfinfo->fd = child_outd;
	if (flags & STATELESS_OUT_STREAM) {
		out_sbuf.setWriter(streamWriter, out_sfinfo);
	} else {
		out_sfinfo->sbuf = &out_sbuf;
		out_sfinfo->stop = &stop_threads;
		out_writer = Thread(bufWriter, out_sfinfo);
	}

	// Handler of err stream
	err_sfinfo->fd = child_errd;
	if (flags & STATELESS_ERR_STREAM) {
		err_sbuf.setWriter(streamWriter, err_sfinfo);
	} else {
		err_sfinfo->sbuf = &err_sbuf;
		err_sfinfo->stop = &stop_threads;
		err_writer = Thread(bufWriter, err_sfinfo);
	}
}

CommandExec::~CommandExec(void)
{
	delete out_sfinfo;
	delete err_sfinfo;
	delete in_sfinfo;
	if (!child_quit) {
		throw std::runtime_error("Child process was not finished!");
	}
}

int CommandExec::wait(void)
{
	HppAssert(!child_quit, "Command has already ended!");
	::waitpid(child_pid, NULL, 0);
	stop_threads = true;
	::close(child_outd);
	::close(child_errd);
	::close(child_ind);
	if (!(flags & STATELESS_OUT_STREAM)) {
		out_writer.wait();
	}
	if (!(flags & STATELESS_ERR_STREAM)) {
		err_writer.wait();
	}
	child_quit = true;
	return child_return;
}

void CommandExec::kill(void)
{
	HppAssert(!child_quit, "Command has already ended!");
	if (::kill(child_pid, SIGKILL) != 0) {
		throw Exception("Unable to kill command!");
	}
	wait();
}

size_t streamWriter(uint8_t* buf, size_t buf_size, void* sfinfo_raw)
{
	StreamfillerInfo* sfinfo = reinterpret_cast< StreamfillerInfo* >(sfinfo_raw);
	int fd = sfinfo->fd;
	ssize_t bytes_read = read(fd, buf, buf_size);
	if (bytes_read < 0) {
		throw Exception("Unable to read command output!");
	}
	return bytes_read;
}

void bufWriter(void* sfinfo_raw)
{
	StreamfillerInfo* sfinfo = reinterpret_cast< StreamfillerInfo* >(sfinfo_raw);
	int fd = sfinfo->fd;
	Streambuf* sbuf = sfinfo->sbuf;
	bool* stop = sfinfo->stop;

	size_t const BUF_SIZE = 64;
	uint8_t buf[BUF_SIZE];

	do {

		ssize_t bytes_read = read(fd, buf, BUF_SIZE);
		if (bytes_read < 0) {
			throw Exception("Unable to read data from pipe!");
		}
		sbuf->pushData(buf, buf + bytes_read);

	} while (!(*stop));

}

}
