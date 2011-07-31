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
	if (widget_under_mouse) {
		widget_under_mouse->mouseEvent(event);
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
