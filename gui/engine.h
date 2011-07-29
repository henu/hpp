#ifndef HPP_GUI_ENGINE_H
#define HPP_GUI_ENGINE_H

namespace Hpp
{

class Viewport;

namespace Gui
{

class Renderer;
class Menubar;

class Engine
{

public:

	Engine(void);
	~Engine(void);

	void render(Renderer* rend);

	void setMenubar(Menubar* menubar);

private:

	bool pos_or_size_changed;

	Menubar* menubar;

};

}

}

#endif
