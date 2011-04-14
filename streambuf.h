#ifndef HPP_STREAMBUF_H
#define HPP_STREAMBUF_H

#include "byteq.h"
#include "condition.h"
#include "mutex.h"
#include "lock.h"

#include <stdint.h>

namespace Hpp
{

class Streambuf
{

public:

	typedef size_t (*WriterCallback)(uint8_t* buf, size_t buf_size, void* data);

	inline Streambuf(void);

	// Push more data
	inline void pushData(uint8_t const* begin, uint8_t const* end);

	// Read bytes. Returns amount of bytes read. Zero means EOF.
	inline size_t readData(uint8_t* buf, size_t buf_size);

	inline void setWriter(WriterCallback writer, void* data);

private:

	ByteQ buf;

	Condition cond;

	Mutex mutex;

	// Writer and it's data
// TODO: Need mutex protect?
	WriterCallback writer;
	void* writer_data;

};

inline Streambuf::Streambuf(void)
{
}

inline void Streambuf::pushData(uint8_t const* begin, uint8_t const* end)
{
	Lock lock(mutex);
	buf.insert(begin, end);
	lock.unlock();
	cond.signal();
}

inline size_t Streambuf::readData(uint8_t* buf, size_t buf_size)
{
	HppAssert(buf_size > 0, "Buffer size must be greater than zero!");
	if (writer) {
		return writer(buf, buf_size, writer_data);
	}
	Lock lock(mutex);
	size_t amount;
	do {
		amount = std::min(buf_size, this->buf.size());
		if (amount == 0) {
			cond.wait(mutex);
			continue;
		}
		break;
	} while (true);
// TODO/Optimize: Do not use loop!
	for (size_t offset = 0;
	     offset < amount;
	     offset ++) {
	     	buf[offset] = this->buf.pop();
	}
	return amount;
}

inline void Streambuf::setWriter(WriterCallback writer, void* data)
{
	this->writer = writer;
	writer_data = data;
}

}

#endif
