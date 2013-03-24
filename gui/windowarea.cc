#include "windowarea.h"

#include "window.h"

namespace Hpp
{

namespace Gui
{

Windowarea::Windowarea(void)
{
}

Windowarea::~Windowarea(void)
{
	// Remove windows
	for (Windows::iterator windows_it = windows.begin();
	     windows_it != windows.end();
	     windows_it ++) {
		Window* window = *windows_it;
		window->setWindowarea(NULL);
	}
}

void Windowarea::addWindow(Window* window)
{
	if (std::find(windows.begin(), windows.end(), window) != windows.end()) {
		throw Exception("Window is already in stack!");
	}
	windows.push_back(window);
	addChild(window);
	window->setWindowarea(this);
}

void Windowarea::doRepositioning(void)
{
	for (Windows::iterator windows_it = windows.begin();
	     windows_it != windows.end();
	     windows_it ++) {
		Window* window = *windows_it;
		repositionChild(window, window->getWindowX(), window->getWindowY(), window->getWindowWidth(), window->getWindowHeight());
	}
}

}

}
