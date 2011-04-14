#ifndef HPP_VIDEORECORDER_H
#define HPP_VIDEORECORDER_H

#include "display.h"
#include "real.h"
#include "time.h"
#include "lock.h"
#include "mutex.h"
#include "condition.h"
#include "exception.h"
#include "cast.h"

namespace Hpp
{

class Videorecorder
{

friend class Display;

public:

	inline ~Videorecorder(void);

	// Use %N in filename to represent frame number. Comes with automatoc
	// '0' padding.
	inline void saveFrames(Path const& basepath, std::string const& filename);

private:

	// Only display is able to create Videorecorders
	inline Videorecorder(Display* disp, Real fps, Display::Recordingmethod rmethod);

	inline void newFrameAvailable(Time frame_time);
	inline Time getOldestNeededFrame(void);

private:

	typedef std::vector< Image > Frames;
	typedef std::vector< Thread > Threads;

	Display* disp;

	// Options
	Real fps;
	Delay dbf; // Delay Between Frames
	Display::Recordingmethod rmethod;
	Time starttime;

	// Time measuring
	Time nframe;		// Next frame to capture
	Time latest_frame;	// Latest frame available.
	Mutex times_mutex;

	// Encoders
	Threads encoders;
	bool encoders_stop;
	Mutex encoders_mutex;
	Condition encoders_cond;

	Frames frames;
	Mutex frames_mutex;

	// Encoder thread
	inline static void encoder(void* recorder_raw);

	inline bool isEncodingPossible(void);

};

inline Videorecorder::~Videorecorder(void)
{
	// Request stop
	Lock encoders_lock(encoders_mutex);
	encoders_stop = true;
	encoders_lock.unlock();
	encoders_cond.broadcast();
	// Wait for threads to finish
	for (Threads::iterator encoders_it = encoders.begin();
	     encoders_it != encoders.end();
	     encoders_it ++) {
	     	encoders_it->wait();
	}
	disp->unregisterVideorecorder(this);
}

void Videorecorder::saveFrames(Path const& basepath, std::string const& filename)
{
	// Wait until there is no more to encode
	while (isEncodingPossible()) {
		sleep(Hpp::Delay::msecs(250));
	}

	Lock frames_lock(frames_mutex);
	uint8_t fullpadding = sizeToStr(frames.size()-1).size();
	for (size_t frame_id = 0; frame_id < frames.size(); frame_id ++) {
		Image const& frame = frames[frame_id];
		std::string filename_fixed;
		for (size_t filename_offset = 0; filename_offset < filename.size(); filename_offset ++) {
			char c = filename[filename_offset];
			if (c != '%') {
				filename_fixed += c;
			} else {
				if (filename_offset == filename.size() - 1) {
					throw Exception("Invalid escapement!");
				}
				filename_offset ++;
				c = filename[filename_offset];
				if (c == 'N') {
					std::string frame_id_str = sizeToStr(frame_id);
					filename_fixed += std::string(fullpadding - frame_id_str.size(), '0') + frame_id_str;
				} else {
					filename_fixed += c;
				}
			}
		}
		Path path = basepath / filename_fixed;
		frame.save(path);
	}
	frames.clear();
}

inline Videorecorder::Videorecorder(Display* disp, Real fps, Display::Recordingmethod rmethod) :
disp(disp),
fps(fps),
dbf(Delay::nsecs((double)1000000000 / (double)fps)),
rmethod(rmethod),
starttime(now()),
nframe(starttime + dbf),
latest_frame(starttime),
encoders_stop(false)
{
	// Start thread
	Thread new_encoder = Thread(encoder, this);
	encoders.push_back(new_encoder);
}

inline void Videorecorder::newFrameAvailable(Time frame_time)
{
	Lock times_lock(times_mutex);
	latest_frame = frame_time;
	times_lock.unlock();
	encoders_cond.broadcast();
}

inline Time Videorecorder::getOldestNeededFrame(void)
{
	Lock times_lock(times_mutex);
	return nframe - dbf * 2;
}

inline void Videorecorder::encoder(void* recorder_raw)
{
	Videorecorder* recorder = reinterpret_cast< Videorecorder* >(recorder_raw);
	Display* disp = recorder->disp;
	Delay dbf = recorder->dbf;
	Display::Recordingmethod rmethod = recorder->rmethod;
	Time& nframe = recorder->nframe;
	Mutex& times_mutex = recorder->times_mutex;
	bool& encoders_stop = recorder->encoders_stop;
	Mutex& encoders_mutex = recorder->encoders_mutex;
	Condition& encoders_cond = recorder->encoders_cond;
	Frames& frames = recorder->frames;
	Mutex& frames_mutex = recorder->frames_mutex;

	Lock encoders_lock(encoders_mutex);
	while (!encoders_stop) {
		encoders_lock.unlock();

		// Check if it is possible to encode new frame
		bool encoding_possible = recorder->isEncodingPossible();

		if (!encoding_possible) {
			encoders_lock.relock();
			encoders_cond.wait(encoders_mutex);
			continue;
		}

		switch (rmethod) {

		case Display::SIMPLE:
			{
				Lock times_lock(times_mutex);
				Image frame = disp->getNextVideoframe(nframe);
				times_lock.unlock();
				Lock frames_lock(frames_mutex);
				frames.push_back(frame);
				frames_lock.unlock();
				times_lock.relock();
				nframe += dbf;
			}
			break;

		case Display::INTERPOLATE_SIMPLE:
			{
				// Get range of frames
				Lock times_lock(times_mutex);
				Display::Rawframes rawframes;
				HppAssert(nframe - dbf < nframe, "Fail!");
				disp->getVideoframesFromRange(rawframes, nframe - dbf, nframe);
				times_lock.unlock();

				HppAssert(!rawframes.empty(), "No frames found!");
				Image const& rawframe0 = rawframes.begin()->second;

				// Encode frame
				size_t result_size = rawframe0.getWidth() * rawframe0.getHeight() * rawframe0.getBytesPerPixel();
				std::vector< size_t > result(result_size, 0);
				for (Display::Rawframes::const_iterator rawframes_it = rawframes.begin();
				     rawframes_it != rawframes.end();
				     rawframes_it ++) {
				     	Image const& rawframe = rawframes_it->second;
				     	ByteV rawframe_data = rawframe.getData();
				     	for (size_t offset = 0; offset < result_size; offset ++) {
				     		result[offset] += rawframe_data[offset];
				     	}
				}
				ByteV result_bv;
				result_bv.reserve(result_size);
				for (size_t offset = 0; offset < result_size; offset ++) {
					size_t byte = result[offset] / rawframes.size();
					HppAssert(byte <= 255, "Too big value!");
					result_bv.push_back(byte);
				}
				Image frame(result_bv, rawframe0.getWidth(), rawframe0.getHeight(), rawframe0.getFormat());

				// Push frame to container
				Lock frames_lock(frames_mutex);
				frames.push_back(frame);
				frames_lock.unlock();
				times_lock.relock();
				nframe += dbf;
			}
			break;

		default:
			HppAssert(false, "Invalid encoding method!");

		}

		encoders_lock.relock();
	}
}

inline bool Videorecorder::isEncodingPossible(void)
{
	switch (rmethod) {

	case Display::SIMPLE:
		{
			Lock times_lock(times_mutex);
			return latest_frame > nframe;
		}

	case Display::INTERPOLATE_SIMPLE:
		{
			Lock times_lock(times_mutex);
			return latest_frame > nframe;
		}

	case Display::INTERPOLATE_SAW:
HppAssert(false, "Not supported!");
		return false;

	}

	HppAssert(false, "Invalid encoding type!");
	return false;
}

}

#endif
