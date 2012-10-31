#include "display.h"

#include "videorecorder.h"
#include "genericmaterial.h"
#include "texturemanager.h"
#include "vbomanager.h"
#include "lock.h"
#include "debug.h"
#include "pixelformat.h"
#include "gldebug.h"
#include "glsystem.h"
#include "inc_gl.h"
#include "assert.h"
#include "exception.h"

#include <algorithm>

namespace Hpp
{

static bool videomodeComparer(Display::Mode const& mode1, Display::Mode const& mode2)
{
	size_t mode1_area = mode1.width * mode1.height;
	size_t mode2_area = mode2.width * mode2.height;
	return mode1_area > mode2_area;
}

Display Display::instance;

Display::Modes Display::getSupportedDisplaymodes(uint8_t bpp, uint8_t flags)
{
	instance.sdlhandler.init();

	SDL_PixelFormat format;
	format.palette = NULL;
	format.BitsPerPixel = bpp;
	format.BytesPerPixel = bpp/8;

	bool fullscreen = flags & FULLSCREEN;
	bool common_modes = flags & COMMON_MODES;

	SDL_Rect** modes = SDL_ListModes(&format, fullscreen * SDL_FULLSCREEN | SDL_OPENGL);
	if (!modes) {
		instance.sdlhandler.deinit();
		return Modes();
	}

	// Check if any mode is available
	if (modes == reinterpret_cast< SDL_Rect** >(-1)) {
// TODO: What should we do here? I suggest we return a collection of basic displaymodes.
HppAssert(false, "Not implemented yet!");
		instance.sdlhandler.deinit();
		return Modes();
	}

	// There was some displaymodes available. Go them through and put them
	// to vector.
	Modes result;
	for (SDL_Rect** modes_it = modes; *modes_it; ++ modes_it) {
		Mode new_mode;
		new_mode.width = (*modes_it)->w;
		new_mode.height = (*modes_it)->h;

		// If only common modes are accepted, then strip those that
		// aren't part of them.
		if (common_modes &&
		    (new_mode.width != 640 || new_mode.height != 480) &&
		    (new_mode.width != 800 || new_mode.height != 600) &&
		    (new_mode.width != 1024 || new_mode.height != 768) &&
		    (new_mode.width != 1280 || new_mode.height != 1024) &&
		    (new_mode.width != 1440 || new_mode.height != 900) &&
		    (new_mode.width != 1600 || new_mode.height != 1024) &&
		    (new_mode.width != 1680 || new_mode.height != 1050) &&
		    (new_mode.width != 1920 || new_mode.height != 1200) &&
		    (new_mode.width != 2560 || new_mode.height != 1600)) {
			continue;
		}
		result.push_back(new_mode);
	}
	// Sort videomodes
	std::sort(result.begin(), result.end(), videomodeComparer);

	instance.sdlhandler.deinit();
	return result;
}

void Display::open(uint32_t width,
                   uint32_t height,
                   uint8_t bpp,
                   std::string title,
                   uint8_t flags)
{

	// Ensure display isn't already opened
	HppAssert(!instance.opened, "Display is already opened!");

	instance.sdlhandler.init();

	// Set ID of main thread
	instance.self_thread_id = getThisThreadID();

	// Gather flags
	bool fullscreen = flags & FULLSCREEN;

	// Set GL attributes
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
		instance.sdlhandler.deinit();
		throw Exception("Unable to set double buffer on!");
	}
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) != 0) {
		instance.sdlhandler.deinit();
		throw(Exception("Unable to set stencil size!"));
	}

	// Check if videomode is available
	instance.bpp = static_cast< unsigned char >(SDL_VideoModeOK(width, height, bpp, fullscreen * SDL_FULLSCREEN | SDL_OPENGL));
	if (instance.bpp == 0) {
		instance.sdlhandler.deinit();
		throw Exception("Requested video mode is not available!");
	} else if (instance.bpp != bpp) {
		instance.sdlhandler.deinit();
		throw Exception("Requested video mode is not available with that bpp!");
	}

	// Open requested videomode
	instance.display = SDL_SetVideoMode(width, height, bpp, fullscreen * SDL_FULLSCREEN | SDL_OPENGL);

	if (instance.display == NULL) {
                instance.sdlhandler.deinit();
		throw Exception("Unable to open requested video mode!");
	}
	HppCheckGlErrors();

	// Reset scissorstack
	instance.sstack.clear();
	glDisable(GL_SCISSOR_TEST);

	// Set dimensions, etc. of display
	instance.opened = true;
	instance.width = width;
	instance.height = height;

	// Set window title
	SDL_WM_SetCaption(title.c_str(), title.c_str());

	// Initialize system
	GlSystem::initialize();
	HppCheckGlErrors();

	// Initialize some shaders
	GenericMaterial::initShaders();

}

void Display::close(void)
{
	HppAssert(instance.opened, "Not opened!");

	GenericMaterial::deinitShaders();

	instance.vbomanager.cleanReleasableVbos();

	SDL_FreeSurface(instance.display);
	instance.display = NULL;
	instance.opened = false;

	instance.sdlhandler.deinit();
}

void Display::beginRendering(void)
{
	HppCheckForCorrectThread();
}

void Display::endRendering(void)
{
	HppCheckForCorrectThread();
	SDL_GL_SwapBuffers();
	instance.vbomanager.cleanReleasableVbos();
	Texturemanager::cleanReleasableTextures();
	// Get screenshot and inform all recorders about it
	if (!instance.recorders.empty()) {
		Image new_frame = getScreenshot();
		Lock rawframes_lock(instance.rawframes_mutex);
		Time time_now = now();
		instance.rawframes[time_now] = new_frame;
		rawframes_lock.unlock();
		Time last_needed = time_now;
		for (Videorecorders::iterator recorders_it = instance.recorders.begin();
		     recorders_it != instance.recorders.end();
		     ++ recorders_it) {
			Videorecorder* recorder = *recorders_it;
			recorder->newFrameAvailable(time_now);
			last_needed = std::min(last_needed, recorder->getOldestNeededFrame());
		}
		// Remove those frames that are no more needed by any
		// Videorecorder
		rawframes_lock.relock();
		while (instance.rawframes.begin()->first < last_needed) {
			instance.rawframes.erase(instance.rawframes.begin());
		}
	}

}

void Display::clearScreen(Color const& color, bool clear_color, bool clear_depth)
{
	if (clear_color) {
		glClearColor(color.getRed(), color.getGreen(), color.getBlue(), 0);
	}
	if (clear_color && clear_depth) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	} else if (clear_color && !clear_depth) {
		glClear(GL_COLOR_BUFFER_BIT);
	} else if (clear_depth) {
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void Display::setMouseVisible(bool visible)
{
	if (visible) {
		SDL_ShowCursor(SDL_ENABLE);
	} else {
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void Display::pushScissor(int32_t pos_x, int32_t pos_y,
                          uint32_t size_x, uint32_t size_y)
{
	if (instance.sstack.empty()) {
		HppAssert(!glIsEnabled(GL_SCISSOR_TEST), "");
		glEnable(GL_SCISSOR_TEST);
	}
	Scissoritem new_item;
	new_item.pos_x = pos_x;
	new_item.pos_y = pos_y;
	new_item.size_x = size_x;
	new_item.size_y = size_y;
	instance.sstack.push_back(new_item);
	instance.setScissorFromStack();
}

void Display::popScissor(void)
{
	HppAssert(!instance.sstack.empty(), "Scissor stack is empty!");
	HppAssert(glIsEnabled(GL_SCISSOR_TEST), "GL_SCISSOR_TEST should be disabled!");
	instance.sstack.pop_back();
	if (instance.sstack.empty()) {
		glDisable(GL_SCISSOR_TEST);
	} else {
		instance.setScissorFromStack();
	}
}

Image Display::getScreenshot(void)
{
	HppCheckForCorrectThread();
	HppAssert(instance.opened, "Display is not opened yet!");
// TODO: Alpha is useless, but otherwise image saving will not work!
	uint8_t* img_buf = new uint8_t[instance.width * instance.height * 4];
	glReadPixels(0, 0, instance.width, instance.height,
	             GL_RGBA, GL_UNSIGNED_BYTE, img_buf);
	Image img(img_buf, instance.width, instance.height, RGBA, Image::DONT_DROP_USELESS_ALPHA | Image::FLIP_VERTICALLY);
	delete[] img_buf;
	return img;
}

Videorecorder* Display::startRecordingVideo(Real fps, Recordingmethod rmethod)
{
	HppCheckForCorrectThread();
	Videorecorder* new_recorder = new Videorecorder(&instance, fps, rmethod);
	HppAssert(instance.recorders.find(new_recorder) == instance.recorders.end(), "Fail!");
	instance.recorders.insert(new_recorder);
	return new_recorder;
}

void Display::unregisterVideorecorder(Videorecorder* recorder)
{
	HppCheckForCorrectThread();
	HppAssert(recorders.find(recorder) != recorders.end(), "Videorecorder not found!");
	recorders.erase(recorder);
	// Check if rawframes should be cleaned
	if (recorders.empty()) {
		Lock rawframes_lock(rawframes_mutex);
		rawframes.clear();
	}
}

Image Display::getNextVideoframe(Time const& timestamp)
{
	Lock rawframes_lock(rawframes_mutex);
	Rawframes::const_iterator rawframes_find = rawframes.upper_bound(timestamp);
	HppAssert(rawframes_find != rawframes.end(), "Frame not found!");
	return rawframes_find->second;
}

void Display::getVideoframesFromRange(Rawframes& result, Time const& min, Time const& max)
{
	HppAssert(result.empty(), "Result must be empty at first!");
	HppAssert(min <= max, "Min must be smaller or equal to max!")
	HppAssert(!rawframes.empty(), "There are no rawframes!");
	Lock rawframes_lock(rawframes_mutex);
	for (Rawframes::const_iterator rawframes_it = rawframes.lower_bound(min);
	     rawframes_it != rawframes.end() && rawframes_it->first < max;
	     ++ rawframes_it) {
	     	result[rawframes_it->first] = rawframes_it->second;
	}
	HppAssert(!result.empty(), "No frames found!");
}

Display::Display(void) :
opened(false),
sdlhandler(Sdlhandler::VIDEO)
{
}

Display::~Display(void)
{
	HppAssert(recorders.empty(), "There are unregistered videorecorders!");
	// Clean
	if (instance.opened) {
		close();
	}
}

void Display::setScissorFromStack(void)
{
	Scissoritem stotal;
	stotal.pos_x = 0;
	stotal.pos_y = 0;
	stotal.size_x = width;
	stotal.size_y = height;
	for (Scissorstack::const_iterator sstack_it = sstack.begin();
	     sstack_it != sstack.end();
	     ++ sstack_it) {
		Scissoritem const& sitem = *sstack_it;
		if (sitem.pos_x > stotal.pos_x) {
			size_t diff = sitem.pos_x - stotal.pos_x;
			if (stotal.size_x < diff) {
				stotal.size_x = 0;
			} else {
				stotal.size_x -= diff;
			}
			stotal.pos_x = sitem.pos_x;
		}
		if (sitem.pos_y > stotal.pos_y) {
			size_t diff = sitem.pos_y - stotal.pos_y;
			if (stotal.size_y < diff) {
				stotal.size_y = 0;
			} else {
				stotal.size_y -= diff;
			}
			stotal.pos_y = sitem.pos_y;
		}
		if (sitem.pos_x + sitem.size_x < stotal.pos_x + stotal.size_x) {
			size_t diff = stotal.pos_x + stotal.size_x - (sitem.pos_x + sitem.size_x);
			if (stotal.size_x < diff) {
				stotal.size_x = 0;
			} else {
				stotal.size_x -= diff;
			}
		}
		if (sitem.pos_y + sitem.size_y < stotal.pos_y + stotal.size_y) {
			size_t diff = stotal.pos_y + stotal.size_y - (sitem.pos_y + sitem.size_y);
			if (stotal.size_y < diff) {
				stotal.size_y = 0;
			} else {
				stotal.size_y -= diff;
			}
		}
	}
	HppAssert(glIsEnabled(GL_SCISSOR_TEST), "");
	glScissor(stotal.pos_x, stotal.pos_y, stotal.size_x, stotal.size_y);

}

}
