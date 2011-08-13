#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"
#include "vectorcontainer.h"
#include "label.h"

#include <string>

namespace Hpp
{

namespace Gui
{

class Filedialog : public Window
{

public:

	enum Type { SAVE, OPEN };

	inline Filedialog(void);
	inline virtual ~Filedialog(void);

	inline void setType(Type type) { this->type = type; }
	inline void setFileextension(std::string const& fileext) { this->fileext = fileext; }
	inline void setSelectMultiple(bool selectmultiple = true) { this->selectmultiple = selectmultiple; }

private:

	// Options
	Type type;
	std::string fileext;
	bool selectmultiple;

	// Widgets
	Vectorcontainer maincontainer;
	Vectorcontainer filenamecontainer;
	Label filenamelabel;

};

inline Filedialog::Filedialog(void) :
type(SAVE),
selectmultiple(false)
{
	// Main container
	maincontainer.setDirection(Vectorcontainer::VERTICAL);
	setContent(&maincontainer);
	// Filename container
	filenamecontainer.setDirection(Vectorcontainer::HORIZONTAL);
	maincontainer.addWidget(&filenamecontainer);
	// Filename label
	filenamelabel.setLabel("Filename:");
	filenamecontainer.addWidget(&filenamelabel);

}

inline Filedialog::~Filedialog(void)
{
}

}

}

#endif

