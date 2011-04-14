#include "joystick.h"

#include "statemanager.h"
#include "assert.h"
#include "exception.h"

namespace Hpp
{

Joystick::Joystick(unsigned int joystick_id) :
id(joystick_id)
{

	if (static_cast< int >(joystick_id) > SDL_NumJoysticks()) {
		throw Exception("Joystick doesn't exist!");
	}

	joy = Statemanager::registerJoystick(this, id);

	// Read amount of axes and buttons.
	unsigned char axes_size = static_cast< unsigned char >(SDL_JoystickNumAxes(joy));
	unsigned char buttons_size = static_cast< unsigned char >(SDL_JoystickNumButtons(joy));

	// Initialize containers of states
	axes.assign(axes_size, 0);
	buttons.assign(buttons_size, false);
}

Joystick::~Joystick(void)
{
	Statemanager::unregisterJoystick(this, id);
}

unsigned char Joystick::getAxesCount(void) const
{
	return static_cast< unsigned char >(axes.size());
}

unsigned char Joystick::getButtonsCount(void) const
{
	return static_cast< unsigned char >(buttons.size());
}

short Joystick::getAxis(unsigned char axis_id) const
{
	HppAssert(axis_id < axes.size(), "Axis does not exist!");
	return axes[axis_id];
}

bool Joystick::getButton(unsigned char button_id) const
{
	HppAssert(button_id < buttons.size(), "Button does not exist!");
	return buttons[button_id];
}



void Joystick::setAxis(unsigned char axis_id, short value)
{
	HppAssert(axis_id < axes.size(), "Axis does not exist!");
	axes[axis_id] = value;
}


void Joystick::setButton(unsigned char button_id, char pressed)
{
	HppAssert(button_id < buttons.size(), "Button does not exist!");
	buttons[button_id] = pressed;
}

}
