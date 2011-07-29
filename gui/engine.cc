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
pos_or_size_changed(true),
menubar(NULL)
{
}

Engine::~Engine(void)
{
}

void Engine::render(Renderer* rend)
{
	// If position or size has changed, then update all child widgets
	if (pos_or_size_changed) {
		if (menubar) {
			menubar->updatePosAndSize(rend, 0, 0, rend->getWidth());
		}
		pos_or_size_changed = false;
	}

	rend->initRendering();

	if (menubar) {
		menubar->render(rend);
	}

	rend->deinitRendering();
}

void Engine::setMenubar(Menubar* menubar)
{
	this->menubar = menubar;
	menubar->setParent(NULL);
	menubar->setEngine(this);
}

}

}
