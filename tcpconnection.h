#ifndef HPP_TCPCONNETION_H
#define HPP_TCPCONNETION_H

#ifdef WIN32
#ifndef HPP_USE_SDL_NET
#define HPP_USE_SDL_NET
#endif
#endif

#include "time.h"
#include "condition.h"
#include "mutex.h"
#include "thread.h"
#include "lock.h"
#include "byteq.h"
#include "assert.h"
#include "cast.h"
#include "noncopyable.h"
#ifdef HPP_USE_SDL_NET
#include <SDL/SDL_net.h>
#endif
#include <string>
#include <vector>
#include <list>

namespace Hpp
{

class TCPReceiver;

class TCPConnection : public NonCopyable
{

	// Friends
	friend class TCPServer;
	friend class Connectionmanager;

public:

	typedef void (*DataReceiver)(TCPConnection* conn, void*);

	// Constructor
	TCPConnection(void);
	TCPConnection(std::string const& host_or_ip, uint16_t port);
	~TCPConnection(void);

	void connect(std::string const& host_or_ip, uint16_t port);

	// Closes connection (by local host).
	inline void close(void);

	inline bool isConnected(void);

	// Sets data receiver. Data receiver is a special object that will be
	// notified when data arrives through TCPConnection.
	void setDataReceiver(DataReceiver receiver, void* data);

	// Gets information about remote host
	uint16_t getPort(void) const;
	std::string getHost(void) const;

	// Checks if there is data available.
	size_t getAmountOfWaitingData(void);

	// Methods for reading specific type of data from socket. Note that
	// reading should NOT be done from separate threads at the same time!
	// Before reading, waitForReading() MUST be called to ensure that data
	// exists for reading and that connection is still alive. If specific
	// amount of data is received succesfully, then true is returned by
	// waitForReading() and if connection is closed, false will be returned.
	bool waitForReading(size_t bytes);
	inline uint8_t readUInt8(void);
	inline uint16_t readUInt16(void);
	inline uint32_t readUInt32(void);
	inline uint64_t readUInt64(void);
	inline int8_t readInt8(void);
	inline int16_t readInt16(void);
	inline int32_t readInt32(void);
	inline int64_t readInt64(void);
	inline float readFloat(void);
	inline ByteV readByteV(size_t size);
	inline std::string readString(size_t size);

	// Send specific types of data. These can be called from any thread.
	void initWrite(void);
	void deinitWrite(void);
	inline void writeUInt8(uint8_t i);
	inline void writeUInt16(uint16_t i);
	inline void writeUInt32(uint32_t i);
	inline void writeUInt64(uint64_t i);
	inline void writeInt8(int8_t i);
	inline void writeInt16(int16_t i);
	inline void writeInt32(int32_t i);
	inline void writeInt64(int64_t i);
	inline void writeFloat(float f);
	inline void writeByteV(ByteV const& v);
	inline void writeString(std::string const& s);

	// Waits until all bytes are sent. This is usefull for example when
	// closing is wanted immediately after some write operations.
	void waitUntilAllDataIsSent(void);

	// Enables lag emulation of received data
	void enableLagEmulation(Delay const& lag);

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Creates new TCPConnection with already opened socket. This is called
	// by friend TCPListener.
	#ifndef HPP_USE_SDL_NET
	TCPConnection(int32_t soc, uint16_t port);
	#else
	TCPConnection(TCPsocket sdlsoc, uint16_t port);
	#endif

private:

	// ----------------------------------------
	// Types
	// ----------------------------------------

	// These are used in lag emulation
	struct TimeAndAmount {
		inline TimeAndAmount(Time const& time, size_t amount) :
		time(time), amount(amount) { }
		Time time;
		size_t amount;
	};
	typedef std::list< TimeAndAmount > TimesAndAmounts;


	// ----------------------------------------
	// Data
	// ----------------------------------------

	// Current data receiver and mutex to protect it's changes
	DataReceiver receiver;
	void* receiver_data;
	Mutex receiver_mutex;

	// ID numbers of threads of this object. These are used only for
	// debugging purposes.
	Thread::Id reader_thread_id;
	Thread::Id writer_thread_id;
	Thread::Id notifier_thread_id;

	// Reader thread and mutex to protect it. The mutex will protect queue
	// of read data. Condition to signal when new data is got is also
	// provided.
	Thread reader_thread;
	Mutex reader_mutex;
	Condition reader_cond;
	// Queue of read data
	ByteQ inbuffer;
	// Another queue of data to be read. This is for DataReceiver to move
	// all data quickly away from normal inbuffer. This gives way for real
	// tcp receiver to get data quickly.
// TODO: In future, it might be a good idea to force reading to be done only through data receiver. Then we could get rid of this stupid Mutex here. It is VERY bad idea to force user to use some stupid data receiver!
	ByteQ inbuffer_rcv;
	Mutex inbuffer_rcv_mutex;

	// Writer thread and mutex to protect it. The mutex will protect queue
	// of data to be sent. Condition is to signal thread that new data is
	// available for sending.
	Thread writer_thread;
	Mutex writer_mutex;
	Condition writer_cond;
	// Queue of data to be sent
	ByteQ outbuffer;
	// Another buffer for pending output and mutex to protect it.
	ByteV outbuffer_pending;
	Mutex outbuffer_pending_mutex;
	Lock* outbuffer_pending_lock;
	// This condition is for waiting that all data is really sent. It uses
	// mutex writer_mutex.
	Condition writecheck_cond;

	// Notifier thread. This is used to notify TCPReceiver when new data
	// arrives from remote host.
	Thread notifier_thread;

	// Boolean to indicate whenever connection is open or not.
	// It is protected with mutex.
	bool connected;
	Mutex connected_mutex;

	// Connection socket
	#ifndef HPP_USE_SDL_NET
	int32_t soc;
	#else
	TCPsocket sdlsoc;
	#endif

	// Host and port of this connection
	std::string host_or_ip;
	uint16_t port;

	// Lag emulation. These are protected by reader_mutex.
	bool lag_emulation;
	Delay lag_emulation_amount;
	TimesAndAmounts lag_emulation_queue;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// Closes connection
	void close(bool closed_by_remote_server);

	// Closes connection by remote host
	inline void closeByRemoteHost(void);

	// Reader thread
	static void readerThread(void* tcpconnection_raw);

	// Writer thread
	static void writerThread(void* tcpconnection_raw);

	// Notifier thread
	static void notifierThread(void* tcpconnection_raw);

};

inline void TCPConnection::close(void)
{
	close(false);
}

inline bool TCPConnection::isConnected(void)
{
	Lock connected_loc(connected_mutex);
	bool result = connected;
	return result;
}

inline uint8_t TCPConnection::readUInt8(void)
{
	HppAssert(inbuffer_rcv.size() >= 1, "Not enough data in input buffer!");
	uint8_t result = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return result;
}

inline uint16_t TCPConnection::readUInt16(void)
{
	HppAssert(inbuffer_rcv.size() >= 2, "Not enough data in input buffer!");
	uint8_t result_bytes[2];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToUInt16(result_bytes);
}

inline uint32_t TCPConnection::readUInt32(void)
{
	HppAssert(inbuffer_rcv.size() >= 4, "Not enough data in input buffer!");
	uint8_t result_bytes[4];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[2] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[3] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToUInt32(result_bytes);
}

inline uint64_t TCPConnection::readUInt64(void)
{
	HppAssert(inbuffer_rcv.size() >= 8, "Not enough data in input buffer!");
	uint8_t result_bytes[8];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[2] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[3] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[4] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[5] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[6] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[7] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToUInt64(result_bytes);
}

inline int8_t TCPConnection::readInt8(void)
{
	HppAssert(inbuffer_rcv.size() >= 1, "Not enough data in input buffer!");
	int8_t result = static_cast< int8_t >(inbuffer_rcv.front());
	inbuffer_rcv.pop();
	return result;
}

inline int16_t TCPConnection::readInt16(void)
{
	HppAssert(inbuffer_rcv.size() >= 2, "Not enough data in input buffer!");
	uint8_t result_bytes[2];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToInt16(result_bytes);
}

inline int32_t TCPConnection::readInt32(void)
{
	HppAssert(inbuffer_rcv.size() >= 4, "Not enough data in input buffer!");
	uint8_t result_bytes[4];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[2] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[3] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToInt32(result_bytes);
}

inline int64_t TCPConnection::readInt64(void)
{
	HppAssert(inbuffer_rcv.size() >= 8, "Not enough data in input buffer!");
	uint8_t result_bytes[8];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[2] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[3] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[4] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[5] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[6] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[7] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToInt64(result_bytes);
}

inline float TCPConnection::readFloat(void)
{
	HppAssert(inbuffer_rcv.size() >= 4, "Not enough data in input buffer!");
	uint8_t result_bytes[4];
	result_bytes[0] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[1] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[2] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	result_bytes[3] = inbuffer_rcv.front();
	inbuffer_rcv.pop();
	return cStrToFloat(result_bytes);
}

inline ByteV TCPConnection::readByteV(size_t size)
{
	HppAssert(inbuffer_rcv.size() >= size, "Not enough data in input buffer!");
	ByteV result;
	result.reserve(size);
	for (size_t bytes_read = 0; bytes_read < size; bytes_read ++) {
		result.push_back(inbuffer_rcv.front());
		inbuffer_rcv.pop();
	}
	return result;
}

inline std::string TCPConnection::readString(size_t size)
{
	HppAssert(inbuffer_rcv.size() >= size, "Not enough data in input buffer!");
	std::string result;
	result.reserve(size);
	for (size_t bytes_read = 0; bytes_read < size; bytes_read ++) {
		result.push_back(inbuffer_rcv.front());
		inbuffer_rcv.pop();
	}
	return result;
}

inline void TCPConnection::writeUInt8(uint8_t i)
{
	outbuffer_pending.push_back(i);
}

inline void TCPConnection::writeUInt16(uint16_t i)
{
	uint8_t buf[2];
	uInt16ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
}

inline void TCPConnection::writeUInt32(uint32_t i)
{
	uint8_t buf[4];
	uInt32ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
	outbuffer_pending.push_back(buf[2]);
	outbuffer_pending.push_back(buf[3]);
}

inline void TCPConnection::writeUInt64(uint64_t i)
{
	uint8_t buf[8];
	uInt64ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
	outbuffer_pending.push_back(buf[2]);
	outbuffer_pending.push_back(buf[3]);
	outbuffer_pending.push_back(buf[4]);
	outbuffer_pending.push_back(buf[5]);
	outbuffer_pending.push_back(buf[6]);
	outbuffer_pending.push_back(buf[7]);
}

inline void TCPConnection::writeInt8(int8_t i)
{
	outbuffer_pending.push_back(static_cast< uint8_t >(i));
}

inline void TCPConnection::writeInt16(int16_t i)
{
	uint8_t buf[2];
	int16ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
}

inline void TCPConnection::writeInt32(int32_t i)
{
	uint8_t buf[4];
	int32ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
	outbuffer_pending.push_back(buf[2]);
	outbuffer_pending.push_back(buf[3]);
}

inline void TCPConnection::writeInt64(int64_t i)
{
	uint8_t buf[8];
	int64ToCStr(i, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
	outbuffer_pending.push_back(buf[2]);
	outbuffer_pending.push_back(buf[3]);
	outbuffer_pending.push_back(buf[4]);
	outbuffer_pending.push_back(buf[5]);
	outbuffer_pending.push_back(buf[6]);
	outbuffer_pending.push_back(buf[7]);
}

inline void TCPConnection::writeFloat(float f)
{
	uint8_t buf[4];
	floatToCStr(f, buf);
	outbuffer_pending.push_back(buf[0]);
	outbuffer_pending.push_back(buf[1]);
	outbuffer_pending.push_back(buf[2]);
	outbuffer_pending.push_back(buf[3]);
}

inline void TCPConnection::writeByteV(ByteV const& v)
{
	outbuffer_pending.insert(outbuffer_pending.end(), v.begin(), v.end());
}

inline void TCPConnection::writeString(std::string const& s)
{
	outbuffer_pending.insert(outbuffer_pending.end(), s.begin(), s.end());
}

inline void TCPConnection::closeByRemoteHost(void)
{
	close(true);
}

}

#endif

