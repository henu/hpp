#ifndef HPP_JOYSTICK_H
#define HPP_JOYSTICK_H

#include <SDL/SDL.h>

#include <vector>

namespace Hpp
{

class Joystick
{

	// Friends
	friend class Statemanager;

public:

	// Constructor and destructor
	Joystick(unsigned int joystick_id = 0);
	~Joystick(void);

	// Returns amount of axes and buttons in joystick
	unsigned char getAxesCount(void) const;
	unsigned char getButtonsCount(void) const;

	// Reads state of joystick
	short getAxis(unsigned char axis_id) const;
	bool getButton(unsigned char button_id) const;

private:

	// ----------------------------------------
	// Functions for friends
	// ----------------------------------------

	// Functions to update states of Joystick. These are called by
	// Statemanager.
	void setAxis(unsigned char axis_id, short value);
	void setButton(unsigned char button_id, char pressed);

private:

	// ID of Joystick
// TODO: Is this useless?
	unsigned int id;

	// State of axes and buttons
	std::vector< short > axes;
	std::vector< bool > buttons;

	// SDL joystick
	SDL_Joystick* joy;

};

}

#endif
