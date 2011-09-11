#ifndef HPP_SDLHANDLER_H
#define HPP_SDLHANDLER_H

#include <stdint.h>
#include <SDL/SDL.h>

namespace Hpp
{

class Sdlhandler
{

public:

	// Flags for which parts of SDL to initialize
	static uint8_t const TIMER =    0x01;
	static uint8_t const VIDEO =    0x02;
	static uint8_t const JOYSTICK = 0x04;

	// Constructor and destructor
	Sdlhandler(uint8_t const flags);
	~Sdlhandler(void);

	// Functions to initialize and deinitialize SDL
	void init(void);
	void deinit(void);

private:

	// Parts of SDL to initialize
	static bool init_timer;
	static bool init_video;
	static bool init_joystick;

	// Counter for SDL initializations
	static uint32_t initcounter;

};

}

#endif
