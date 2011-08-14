#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"
#include "vectorcontainer.h"
#include "label.h"
#include "textinput.h"

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
	Vectorcontainer buttonscontainer;
	Label filenamelabel;
	Textinput filenameinput;
// TODO: Change to correct types!
	Label newfolderbutton;
	Label folderview;
	Label cancelbutton;
	Label savebutton;

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
	filenamecontainer.setHorizontalExpanding(1);
	maincontainer.addWidget(&filenamecontainer);
	// Filename label
	filenamelabel.setLabel("Filename:");
	filenamecontainer.addWidget(&filenamelabel);
	// Filename label
	filenameinput.setHorizontalExpanding(1);
	filenamecontainer.addWidget(&filenameinput);
	// New folder button
	newfolderbutton.setLabel("New folder...");
	newfolderbutton.setHorizontalAlignment(Containerwidget::RIGHT);
	maincontainer.addWidget(&newfolderbutton);
	// Folderview
	folderview.setLabel("FOLDERVIEW HERE!");
	folderview.setHorizontalExpanding(1);
	folderview.setVerticalExpanding(1);
	maincontainer.addWidget(&folderview);
	// Filename container
	buttonscontainer.setDirection(Vectorcontainer::HORIZONTAL);
	buttonscontainer.setHorizontalExpanding(1);
	maincontainer.addWidget(&buttonscontainer);
	// Cancel and Save buttons
	cancelbutton.setLabel("Cancel");
	savebutton.setLabel("Save");
	buttonscontainer.addWidget(&cancelbutton);
	buttonscontainer.addWidget(&savebutton);
}

inline Filedialog::~Filedialog(void)
{
}

}

}

#endif

