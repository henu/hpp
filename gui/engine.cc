#include "engine.h"

#include "renderer.h"
#include "menubar.h"
#include "../viewport.h"

#include <cstdlib>

namespace Hpp
{

namespace Gui
{

Engine::Engine(void) :
rend(NULL),
pos_or_size_changed(true),
mouseover_widget(NULL),
menubar(NULL)
{
}

Engine::~Engine(void)
{
}

void Engine::render(void)
{
	// If position or size has changed, then update all child widgets
	if (pos_or_size_changed) {
		if (menubar) {
			menubar->updatePosAndSize(0, 0, rend->getWidth());
		}
		pos_or_size_changed = false;
	}

	rend->initRendering();

	if (menubar) {
		menubar->render();
	}

	rend->deinitRendering();
}

void Engine::setMenubar(Menubar* menubar)
{
	this->menubar = menubar;
	menubar->setEngine(this);
	menubar->setParent(NULL);
}

bool Engine::mouseEvent(Event const& event)
{
	mouse_lastpos_x = event.x;
	mouse_lastpos_y = event.y;

	// Try to get widget over mouse
	Widget* widget_under_mouse = NULL;
	if (menubar) {
		widget_under_mouse = menubar->mouseOverRecursive(event.x, event.y);
	}
// TODO: Code finding widgets from windows!

	// Check if some Widget is interested about these events
	if (event.type == Event::MOUSE_KEY_DOWN) {
		for (MouseClickListeners::iterator mouseclicklisteners_it = mouseclicklisteners.begin();
		     mouseclicklisteners_it != mouseclicklisteners.end();
		     mouseclicklisteners_it ++) {
			Mousekey::KeycodeFlags flags = mouseclicklisteners_it->second;
			if ((event.mousekey == Mousekey::LEFT && (flags & Mousekey::FLAG_LEFT)) ||
			    (event.mousekey == Mousekey::MIDDLE && (flags & Mousekey::FLAG_MIDDLE)) ||
			    (event.mousekey == Mousekey::RIGHT && (flags & Mousekey::FLAG_RIGHT)) ||
			    (event.mousekey == Mousekey::WHEEL_UP && (flags & Mousekey::FLAG_WHEEL_UP)) ||
			    (event.mousekey == Mousekey::WHEEL_DOWN && (flags & Mousekey::FLAG_WHEEL_DOWN))) {
				mouseclicklisteners_it->first->onMouseKeyDownOther(widget_under_mouse, event.x, event.y, event.mousekey);
			}
		}
	}

	// Use event
	if (widget_under_mouse) {
		return widget_under_mouse->mouseEvent(event);
	}
	// If no widget was found, then make sure
	// no Widget thinks its under mouse.
	else {
		setMouseOver(NULL);
	}
	return false;
}

void Engine::registerWidget(Widget* widget)
{
	HppAssert(widgets.find(widget) == widgets.end(), "Widget is already registered to Engine!");
	widgets.insert(widget);
}

void Engine::unregisterWidget(Widget* widget)
{
	HppAssert(widgets.find(widget) != widgets.end(), "Widget is not registered to Engine!");
	widgets.erase(widget);
	// Check if this Widget is listening for mouse clicks
	mouseclicklisteners.erase(widget);
	// Check if this Widget was under mouse
	if (mouseover_widget == widget) {
		mouseover_widget = NULL;
		checkForNewMouseOver();
	}
}

void Engine::setMouseOver(Widget* widget)
{
	if (mouseover_widget) {
		mouseover_widget->setMouseOut(mouse_lastpos_x, mouse_lastpos_y);
	}
	mouseover_widget = widget;
}

void Engine::registerMouseClickListener(Widget* widget, Mousekey::KeycodeFlags flags)
{
	if (flags == 0) {
		mouseclicklisteners.erase(widget);
	} else {
		mouseclicklisteners[widget] = flags;
	}
}

void Engine::checkForNewMouseOver(void)
{
	Widget* widget_under_mouse = NULL;
	if (menubar) {
		widget_under_mouse = menubar->mouseOverRecursive(mouse_lastpos_x, mouse_lastpos_y);
	}
	if (widget_under_mouse) {
		widget_under_mouse->setMouseOver(mouse_lastpos_x, mouse_lastpos_y);
	}
}

}

}
