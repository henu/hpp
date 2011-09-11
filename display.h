#ifndef HPP_DISPLAY_H
#define HPP_DISPLAY_H

#include "inc_gl.h"
#include "image.h"
#include "sdlhandler.h"
#include "time.h"
#include "mutex.h"
#include "thread.h"
#include "real.h"

#include <SDL/SDL.h>
#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>


namespace Hpp
{

class Videorecorder;

class Display
{

	friend class Videorecorder;

public:

	// Type and container of display modes
	struct Mode {
		uint32_t width;
		uint32_t height;
	};
	typedef std::vector< Mode > Modes;

	enum Recordingmethod { SIMPLE,
	                       INTERPOLATE_SIMPLE,
	                       INTERPOLATE_SAW };

	// Flags
	static uint8_t const FULLSCREEN = 0x01;
	static uint8_t const COMMON_MODES = 0x02;

	// Gets list of available displaymodes while in fullscreen
	Modes getSupportedDisplaymodes(uint8_t bpp, uint8_t flags = 0) const;

	// Functions to open and close display
	void static open(uint32_t width,
	                 uint32_t height,
	                 uint8_t bpp,
	                 std::string title = "",
	                 uint8_t flags = 0);
	void static close(void);

	// Checks if this thread is the rendering thread
	inline static bool isThisRenderingThread(void) { return instance.self_thread_id == getThisThreadID(); }

	// Prepares and finsihes everything for rendering process.
	static void beginRendering(void);
	static void endRendering(void);

	// Functions to get size and bpp of Display
	inline static uint32_t getWidth(void) { return instance.width; }
	inline static uint32_t getHeight(void) { return instance.height; }
	inline static uint8_t getBpp(void) { return instance.bpp; }

	// Sets visibility of mouse
	static void setMouseVisible(bool visible = true);

	// Pushes pops scissored area from stack
// TODO: Maybe this class is not a best place for this!
	static void pushScissor(int32_t pos_x, int32_t pos_y,
	                        uint32_t size_x, uint32_t size_y);
	static void popScissor(void);

	static Image getScreenshot(void);

	static Videorecorder* startRecordingVideo(Real fps, Recordingmethod rmethod);

private:

	// ----------------------------------------
	// Functions and types for friends
	// ----------------------------------------

	typedef std::map< Time, Image > Rawframes;

	void unregisterVideorecorder(Videorecorder* recorder);

	// Returns frame that has greater timestamp than given
	Image getNextVideoframe(Time const& timestamp);
	// Returns frames that have time equal or greater than min and smaller
	// than max.
	void getVideoframesFromRange(Rawframes& result, Time const& min, Time const& max);

private:

	// ----------------------------------------
	// Private types
	// ----------------------------------------

	// Types for scissor stack
	struct Scissoritem
	{
		int32_t pos_x, pos_y;
		uint32_t size_x, size_y;
	};
	typedef std::vector< Scissoritem > Scissorstack;

	typedef std::set< Videorecorder* > Videorecorders;


	// ----------------------------------------
	// Data
	// ----------------------------------------

	// The only instance of this class
	static Display instance;

	// State of display
	bool opened;

	// Thread ID of display.
	Thread::Id self_thread_id;

	// Dimensions and bpp
	uint32_t width, height;
	uint8_t bpp;

	// Pointer to the SDL_Surface of display
	SDL_Surface* display;

	// SDL handler object
	Sdlhandler sdlhandler;

	Scissorstack sstack;

	// Videorecorders and raw frames for them
	Videorecorders recorders;
	Rawframes rawframes;
	Mutex rawframes_mutex;

	// Constructor and destructor in private to prevent other instances of
	// this class.
	Display(void);
	~Display(void);

	// Uses scissor stack to set gl scissor. GL_SCISSOR_TEST must be
	// enabled.
	void setScissorFromStack(void);

};

}

#endif
