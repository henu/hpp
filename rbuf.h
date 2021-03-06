#ifndef HPP_RBUF_H
#define HPP_RBUF_H

#include "assert.h"

#include <string.h>
#include <cstdlib>

namespace Hpp
{

template< typename T >
class RBuf
{

public:

	inline RBuf(void) :
	res(0),
	items(0)
	{
	}

	inline ~RBuf(void)
	{
		if (res > 0) {
			delete[] buf;
		}
	}

	inline void clear(void)
	{
		if (res > 0) {
			delete[] buf;
		}
		res = 0;
		items = 0;
	}

	inline bool empty(void) const
	{
		return items == 0;
	}

	inline size_t size(void) const
	{
		return items;
	}

	inline void insert(T const* begin, T const* end)
	{
		HppAssert(end >= begin, "Invalid range!");
		if (begin == end) {
			return;
		}
		size_t add = end - begin;
		ensureSpace(items + add);
		if (write + add <= buf + res) {
			memcpy(write, begin, add * sizeof(T));
			write += add;
		} else {
			size_t amount = buf + res - write;
			memcpy(write, begin, amount * sizeof(T));
			HppAssert(add >= amount, "Fail!");
			size_t amount2 = add - amount;
			memcpy(buf, begin + amount, amount2 * sizeof(T));
			write = buf + amount2;
		}
		items += add;
	}

	inline T pop(void)
	{
		HppAssert(items > 0, "Queue is empty!");
		HppAssert(read != buf + res, "Fail!");
		T result = *read;
		read ++;
		if (read == buf + res) {
			read = buf;
		}
		items --;
// TODO: Check if it would be good idea to reduce space!
		return result;
	}

	inline void push(T const& t)
	{
		ensureSpace(items + 1);
		HppAssert(write != buf + res, "Write points to the end of buffer!");
		*write = t;
		write ++;
		if (write == buf + res) {
			write = buf;
		}
		items ++;
	}

	inline T front(void) const
	{
		HppAssert(items > 0, "No items!");
		return *read;
	}

	inline void swap(RBuf< T >& rbuf)
	{
		size_t swap_res = rbuf.res;
		size_t swap_items = rbuf.items;
		T* swap_write = rbuf.write;
		T* swap_read = rbuf.read;
		T* swap_buf = rbuf.buf;

		rbuf.res = res;
		rbuf.items = items;
		rbuf.write = write;
		rbuf.read = read;
		rbuf.buf = buf;

		res = swap_res;
		items = swap_items;
		write = swap_write;
		read = swap_read;
		buf = swap_buf;
	}

private:

	size_t res;
	size_t items;
	T* write;
	T* read;
	T* buf;

	inline void ensureSpace(size_t req)
	{
		if (res >= req) {
			return;
		}
		req *= 2;
		T* newbuf = new T[req];
		if (items > 0) {
			if (read < write || write == buf) {
				HppAssert(read + items <= buf + res, "Overflow!");
				memcpy(newbuf, read, items * sizeof(T));
			} else {
				size_t amount = buf + res - read;
				memcpy(newbuf, read, amount * sizeof(T));
				HppAssert(items >= amount, "Fail!");
				size_t amount2 = items - amount;
				memcpy(newbuf + amount, buf, amount2 * sizeof(T));
			}
		}
		if (res > 0) {
			delete[] buf;
		}
		res = req;
		buf = newbuf;
		read = newbuf;
		write = newbuf + items;
	}

};

}

#endif
