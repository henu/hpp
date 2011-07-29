#include "menu.h"

#include "renderer.h"

#include <iostream>
namespace Hpp
{

namespace Gui
{

Menu::Menu(void)
{
}

Menu::~Menu(void)
{
}

void Menu::updatePosAndSize(Renderer* rend, int32_t x, int32_t y)
{
	setPosition(x, y);
	setSize(rend->getMenuLabelWidth(label), rend->getMenubarHeight());
}

uint32_t Menu::getLabelWidth(Renderer* rend)
{
	return rend->getMenuLabelWidth(label);
}

void Menu::render(Renderer* rend)
{
	// Label
	rend->renderMenuLabel(this, label);
}

}

}
