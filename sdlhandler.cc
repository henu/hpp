#include "sdlhandler.h"

#include "assert.h"
#include "exception.h"
#include <string>

namespace Hpp
{

bool Sdlhandler::init_timer = false;
bool Sdlhandler::init_video = false;
bool Sdlhandler::init_joystick = false;
uint32_t Sdlhandler::initcounter = 0;

Sdlhandler::Sdlhandler(uint8_t const flags)
{
	// Ensure no initializations has been made
	HppAssert(initcounter == 0, "Unexpected intializations found!");
	if (flags & TIMER) {
		init_timer = true;
	}
	if (flags & VIDEO) {
		init_video = true;
	}
	if (flags & JOYSTICK) {
		init_joystick = true;
	}
}

Sdlhandler::~Sdlhandler(void)
{
}

void Sdlhandler::init(void)
{

	// Do the actual initialization only at the first init
	if (initcounter == 0) {
		if (SDL_Init(SDL_INIT_TIMER * init_timer |
		             SDL_INIT_VIDEO * init_video |
		             SDL_INIT_JOYSTICK * init_joystick) == -1) {
			throw Exception(std::string("Unable to initialize SDL! Reason: ") + SDL_GetError());
		}
	}

	initcounter ++;

}

void Sdlhandler::deinit(void)
{
	initcounter --;

	// Do the actual deinitialization only at the last deinit
	if (initcounter == 0) {
		SDL_Quit();
	}
}

}
