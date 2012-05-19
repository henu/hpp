#include "engine.h"

#include "renderer.h"
#include "widget.h"

#include <algorithm>
#include <cstdlib>

namespace Hpp
{

namespace Gui
{

Engine::Engine(void) :
rend(NULL),
keyboardlistener(NULL),
mouseover_widget(NULL),
content(NULL)
{
}

Engine::~Engine(void)
{
}

void Engine::setRenderer(Renderer* rend)
{
	if (this->rend) {
		rend->setEngine(NULL);
	}
	this->rend = rend;
	if (rend) {
		rend->setEngine(this);
		updateSizes();
	}
}

void Engine::render(void)
{
	HppAssert(rend, "Render not set!");

	if (!content) {
		return;
	}

	rend->initRendering();

	content->render(0, 0);

	rend->deinitRendering();
}

void Engine::setContent(Widget* widget)
{
	if (content) {
		content->setEngine(NULL);
		content->setParent(NULL);
	}
	content = widget;
	content->setEngine(this);
	content->setParent(NULL);
	updateSizes();
}

bool Engine::mouseEvent(Event const& event)
{
	mouse_lastpos_x = event.x;
	mouse_lastpos_y = event.y;

	// Try to get widget over mouse
	Widget* widget_under_mouse = NULL;
	if (content) {
		widget_under_mouse = content->mouseOverRecursive(0, 0, event.x, event.y);
	}

	// Check if some Widget is interested about these events.
	// Widgets needs to be looped in a special way, so they can
	// alter these containers from these callback funtions
	if (event.type == Event::MOUSE_KEY_DOWN) {
		Widget* listener = NULL;
		MouseEventListeners::iterator mouseclicklisteners_it;
		while ((mouseclicklisteners_it = mouseclicklisteners.upper_bound(listener)) != mouseclicklisteners.end()) {
			listener = mouseclicklisteners_it->first;
			Mousekey::KeycodeFlags flags = mouseclicklisteners_it->second;
			if ((event.mousekey == Mousekey::LEFT && (flags & Mousekey::FLAG_LEFT)) ||
			    (event.mousekey == Mousekey::MIDDLE && (flags & Mousekey::FLAG_MIDDLE)) ||
			    (event.mousekey == Mousekey::RIGHT && (flags & Mousekey::FLAG_RIGHT)) ||
			    (event.mousekey == Mousekey::WHEEL_UP && (flags & Mousekey::FLAG_WHEEL_UP)) ||
			    (event.mousekey == Mousekey::WHEEL_DOWN && (flags & Mousekey::FLAG_WHEEL_DOWN))) {
				listener->onMouseKeyDownOther(widget_under_mouse, event.x - listener->getAbsolutePositionX(), event.y - listener->getAbsolutePositionY(), event.mousekey);
			}
		}
	} else if (event.type == Event::MOUSE_KEY_UP) {
		Widget* listener = NULL;
		MouseEventListeners::iterator mousereleaselisteners_it;
		while ((mousereleaselisteners_it = mousereleaselisteners.upper_bound(listener)) != mousereleaselisteners.end()) {
			listener = mousereleaselisteners_it->first;
			Mousekey::KeycodeFlags flags = mousereleaselisteners_it->second;
			if ((event.mousekey == Mousekey::LEFT && (flags & Mousekey::FLAG_LEFT)) ||
			    (event.mousekey == Mousekey::MIDDLE && (flags & Mousekey::FLAG_MIDDLE)) ||
			    (event.mousekey == Mousekey::RIGHT && (flags & Mousekey::FLAG_RIGHT)) ||
			    (event.mousekey == Mousekey::WHEEL_UP && (flags & Mousekey::FLAG_WHEEL_UP)) ||
			    (event.mousekey == Mousekey::WHEEL_DOWN && (flags & Mousekey::FLAG_WHEEL_DOWN))) {
				listener->onMouseKeyUpOther(widget_under_mouse, event.x - listener->getAbsolutePositionX(), event.y - listener->getAbsolutePositionY(), event.mousekey);
			}
		}
	} else if (event.type == Event::MOUSE_MOVE) {
		Widget* listener = NULL;
		Widgets::iterator mousemovelisteners_it;
		while ((mousemovelisteners_it = mousemovelisteners.upper_bound(listener)) != mousemovelisteners.end()) {
			listener = *mousemovelisteners_it;
			listener->onMouseMove(event.x - listener->getAbsolutePositionX(), event.y - listener->getAbsolutePositionY());
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

bool Engine::keyboardEvent(Event const& event)
{
	if (keyboardlistener) {
		// TODO: Also check if there are global listeners, like CTRL+S, CTRL+X, etc.!
		if (event.type == Event::KEY_DOWN) {
			keyboardlistener->onKeyDown(event.key, event.unicode);
		}
		return true;
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
	// Check if this Widget is listening for mouse events
	mouseclicklisteners.erase(widget);
	mousereleaselisteners.erase(widget);
	mousemovelisteners.erase(widget);
	if (keyboardlistener == widget) {
		keyboardlistener = NULL;
	}
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

void Engine::registerMouseReleaseListener(Widget* widget, Mousekey::KeycodeFlags flags)
{
	if (flags == 0) {
		mousereleaselisteners.erase(widget);
	} else {
		mousereleaselisteners[widget] = flags;
	}
}

void Engine::registerMouseMoveListener(Widget* widget, bool listen)
{
	if (listen) {
		mousemovelisteners.insert(widget);
	} else {
		mousemovelisteners.erase(widget);
	}
}

void Engine::registerKeyboardListener(Widget* widget)
{
	if (keyboardlistener && keyboardlistener != widget) {
		keyboardlistener->onKeyboardListeningStop();
	}
	keyboardlistener = widget;
}

bool Engine::amIListeningKeyboard(Widget const* widget) const
{
	return keyboardlistener == widget;
}

void Engine::pushRenderarealimit(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	if (rend) {
		// Tune variables to be at proper area.
		if (x < 0) {
			if ((int32_t)width < -x) width = 0;
			else width += x;
			x = 0;
		}
		if (y < 0) {
			if ((int32_t)height < -y) height = 0;
			else height += y;
			y = 0;
		}
		if ((uint32_t)x + width > rend->getWidth()) {
			if ((uint32_t)x > rend->getWidth()) {
				x = 0;
				width = 0;
			}
			else width = rend->getWidth() - x;
		}
		if ((uint32_t)y + height > rend->getHeight()) {
			if ((uint32_t)y > rend->getHeight()) {
				y = 0;
				height = 0;
			}
			else height = rend->getHeight() - y;
		}
	} else {
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}
	Renderarealimit new_limit;
	new_limit.x = x;
	new_limit.y = y;
	new_limit.width = width;
	new_limit.height = height;
	renderarealimits.push_back(new_limit);
	updateTotalRenderarelimit();
}

void Engine::popRenderarealimit(void)
{
	renderarealimits.pop_back();
	updateTotalRenderarelimit();
}

void Engine::checkForNewMouseOver(void)
{
	if (content) {
		mouseover_widget = content->mouseOverRecursive(0, 0, mouse_lastpos_x, mouse_lastpos_y);
	}
}

void Engine::updateSizes(void)
{
	// If there is no Renderer, then do nothing
	if (!rend) {
		return;
	}

	if (content) {
		content->setSize(rend->getWidth(), rend->getHeight());
		content->setPosition(0, 0);
		content->updateEnvironment();
	}
}

void Engine::updateTotalRenderarelimit(void)
{
	if (!rend) return;

	uint32_t total_x = 0;
	uint32_t total_y = 0;
	uint32_t total_width = rend->getWidth();
	uint32_t total_height = rend->getHeight();

	for (Renderarealimits::const_iterator renderarealimits_it = renderarealimits.begin();
	     renderarealimits_it != renderarealimits.end();
	     renderarealimits_it ++) {
		Renderarealimit const& renderarealimit = *renderarealimits_it;
		if (total_x < renderarealimit.x) {
			if (total_x + total_width < renderarealimit.x) total_width = 0;
			else total_width -= renderarealimit.x - total_x;
			total_x = renderarealimit.x;
		}
		if (total_y < renderarealimit.y) {
			if (total_y + total_height < renderarealimit.y) total_height = 0;
			else total_height -= renderarealimit.y - total_y;
			total_y = renderarealimit.y;
		}
		if (total_x + total_width > renderarealimit.x + renderarealimit.width) {
			if (renderarealimit.x + renderarealimit.width < total_x) total_width = 0;
			else total_width = (renderarealimit.x + renderarealimit.width) - total_x;
		}
		if (total_y + total_height > renderarealimit.y + renderarealimit.height) {
			if (renderarealimit.y + renderarealimit.height < total_y) total_height = 0;
			else total_height = (renderarealimit.y + renderarealimit.height) - total_y;
		}
	}

	if (total_x == 0 && total_y == 0 &&
	    total_width == rend->getWidth() &&
	    total_height == rend->getHeight()) {
		rend->removeRenderareaLimit();
	} else {
		rend->setRenderareaLimit(total_x, total_y, total_width, total_height);
	}

}

}

}
