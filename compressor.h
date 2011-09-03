#ifndef HPP_COMPRESSOR_H
#define HPP_COMPRESSOR_H

#include "bytev.h"
#include "byteq.h"
#include <zlib.h>

namespace Hpp
{

class Compressor
{

public:

	// Compression rate constants
	static int const NO_COMPRESSION = Z_NO_COMPRESSION;
	static int const FAST = Z_BEST_SPEED;
	static int const DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION;
	static int const BEST = Z_BEST_COMPRESSION;

	// Constructor and destructor
	inline Compressor(void);
	inline ~Compressor(void);

	// Initializes compression
	inline void init(int level = DEFAULT_COMPRESSION);

	// Compress a chunk of data
	inline void compress(ByteV const& data);

	// Reads a piece of compressed data. If max_size is set to 0, then
	// maximum amount of compressed data is returned.
	inline ByteV read(size_t max_size = 0);

	// Deinitializes compression and returns the remaining bytes.
	inline ByteV deinit(void);

private:

	// Sizes of dataholders
	static size_t const INPUT_SIZE = 2048;
	static size_t const OUTPUT_SIZE = 2048;

	// Is compressor initialized
	bool initialized;

	// The ZLib Stream object.
	z_streamp zstrm;

	// C-arrays of input and output data.
	Bytef* input;
	Bytef* output;

	// Pending output buffer
	ByteQ output_queue;

};

inline Compressor::Compressor(void) :
initialized(false)
{
	// Allocate space for data holders
	input = new(std::nothrow) Bytef[INPUT_SIZE];
	output = new(std::nothrow) Bytef[OUTPUT_SIZE];
	if (!input || !output) {
		delete input;
		delete output;
		throw std::bad_alloc();
	}

	zstrm = new z_stream;
	// Tune allocation of zstream
	zstrm->zalloc = Z_NULL;
	zstrm->zfree = Z_NULL;
	zstrm->next_in = input;
	zstrm->avail_in = 0;
	zstrm->next_out = output;
	zstrm->avail_out = OUTPUT_SIZE;
}

inline Compressor::~Compressor(void)
{
	HppAssert(!initialized, "");
	delete[] input;
	delete[] output;
	delete zstrm;
}

inline void Compressor::init(int level)
{
	HppAssert(!initialized, "Compressor is already initialized!");
	// Tune data containers of zstream
	zstrm->next_in = input;
	zstrm->avail_in = 0;
	zstrm->next_out = output;
	zstrm->avail_out = OUTPUT_SIZE;
	// Initialize ZStream.
	int err = deflateInit(zstrm, level);
	if (err == Z_MEM_ERROR) {
		throw std::bad_alloc();
	}
	if (err == Z_STREAM_ERROR) {
		throw Exception("Invalid compression level!");
	}
	if (err == Z_VERSION_ERROR) {
		throw Exception("Invalid zlib version!");
	}

	initialized = true;
}

inline void Compressor::compress(ByteV const& data)
{
	HppAssert(initialized, "Compressor is not initialized!");
	size_t data_compressed = 0;
	while (data_compressed < data.size()) {

		// Put as much data as possible to input buffer
		size_t input_used = zstrm->next_in - input + zstrm->avail_in;
		size_t copy_size;
		if (INPUT_SIZE - input_used <= (data.size() - data_compressed)) {
			copy_size = INPUT_SIZE - input_used;
		} else {
			copy_size = data.size() - data_compressed;
		}
		memcpy(zstrm->next_in + zstrm->avail_in,
		       &data[data_compressed],
		       copy_size);
		zstrm->avail_in += copy_size;
		data_compressed += copy_size;
		HppAssert(data_compressed <= data.size(), "");

		// Compress only if the whole input buffer is full.
		HppAssert(input_used + copy_size <= INPUT_SIZE, "");
		if (input_used + copy_size == INPUT_SIZE) {
			#ifndef NDEBUG
			int err = deflate(zstrm, false);
			#else
			deflate(zstrm, false);
			#endif
			HppAssert(err != Z_STREAM_ERROR, "");
			HppAssert(err != Z_BUF_ERROR || zstrm->avail_out == 0, "");
			if (zstrm->next_in == input + INPUT_SIZE) {
				zstrm->next_in = input;
				zstrm->avail_in = 0;
			}
		}

		// If output is full, then empty it to queue
		if (zstrm->avail_out == 0) {
			Bytef* output_end = output + OUTPUT_SIZE;
			HppAssert(zstrm->next_out == output_end, "");
			for (Bytef* output_it = output;
			     output_it != output_end;
			     output_it ++) {
				output_queue.push(*output_it);
			}
			zstrm->next_out = output;
			zstrm->avail_out = OUTPUT_SIZE;
		}

	}

}

inline ByteV Compressor::read(size_t max_size)
{
	HppAssert(initialized, "Compressor is not initialized!");
	ByteV result;
	if (max_size) {
		result.reserve(max_size);
	} else {
		result.reserve(output_queue.size() + OUTPUT_SIZE);
	}

	do {

		// First move everything from queue
		size_t from_queue;
		if (!max_size ||
		    output_queue.size() < max_size) {
			from_queue = output_queue.size();
		} else {
			HppAssert(max_size, "");
			from_queue = max_size;
		}
		for (size_t getter = 0; getter < from_queue; getter ++) {
			HppAssert(!output_queue.empty(), "");
			result.push_back(output_queue.front());
			output_queue.pop();
		}

		// Stop now if we have read enough
		HppAssert(!max_size || result.size() <= max_size, "");
		if (max_size && result.size() == max_size) {
			break;
		}

		// Read output from zstream.
		size_t zstrm_size = zstrm->next_out - output;
		if (!max_size ||
		    zstrm_size <= max_size - result.size()) {
			result.insert(result.end(), output, zstrm->next_out);
			zstrm->next_out = output;
			zstrm->avail_out = OUTPUT_SIZE;
			// All data is now used, so we can stop
			break;
		} else {
			HppAssert(max_size, "");
			size_t copy_size = max_size - result.size();
			HppAssert(copy_size <= zstrm_size, "");
			result.insert(result.end(), output, output + copy_size);
			memmove(output, output + copy_size, zstrm_size - copy_size);
			zstrm->next_out -= copy_size;
			zstrm->avail_out += copy_size;
		}

	} while (!max_size ||
	         result.size() < max_size);

	return result;

}

inline ByteV Compressor::deinit(void)
{
	HppAssert(initialized, "Compressor is not initialized!");
	ByteV result;
	result.reserve(output_queue.size() + OUTPUT_SIZE);
	// First move everything from queue
	while (!output_queue.empty()) {
		result.push_back(output_queue.front());
		output_queue.pop();
	}
	while (true) {

		int err = deflate(zstrm, Z_FINISH);
		HppAssert(err != Z_STREAM_ERROR, "");
		HppAssert(err != Z_BUF_ERROR, "");

		// Move all data from output to result
		result.insert(result.end(), output, zstrm->next_out);
		zstrm->next_out = output;
		zstrm->avail_out = OUTPUT_SIZE;

		// Check if all data was succesfully compressed
		if (err == Z_STREAM_END) {
			HppAssert(zstrm->avail_in == 0, "");
			break;
		}
	}
	#ifndef NDEBUG
	int err = deflateEnd(zstrm);
	#else
	deflateEnd(zstrm);
	#endif
	HppAssert(err != Z_STREAM_ERROR, "");
	HppAssert(err != Z_DATA_ERROR, "");

	initialized = false;

	return result;
}

}

#endif
