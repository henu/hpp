#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"
#include "vectorcontainer.h"
#include "button.h"
#include "label.h"
#include "textinput.h"
#include "folderview.h"

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
	Vectorcontainer pathfoldercontainer;
	Vectorcontainer buttonscontainer;
	Label filenamelabel;
	Textinput filenameinput;
// TODO: Change to correct types!
	Textinput pathinput;
	Button newfolderbutton;
	Folderview folderview;
	Button cancelbutton;
	Button savebutton;

};

inline Filedialog::Filedialog(void) :
type(SAVE),
selectmultiple(false)
{
	Path maps_path = Path::getConfig() / "hme_mapeditor" / "levels";
	ensurePathExists(maps_path);

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
	// Filename input
	filenameinput.setHorizontalExpanding(1);
	filenamecontainer.addWidget(&filenameinput);
	// Path and new folder container
	pathfoldercontainer.setDirection(Vectorcontainer::HORIZONTAL);
	pathfoldercontainer.setHorizontalExpanding(1);
	maincontainer.addWidget(&pathfoldercontainer);
	// Path input
	pathinput.setValue(maps_path.toString(true));
	pathinput.setHorizontalExpanding(1);
	pathfoldercontainer.addWidget(&pathinput);
	// New folder button
	newfolderbutton.setLabel("New folder...");
	pathfoldercontainer.addWidget(&newfolderbutton);
	// Folderview
	folderview.setHorizontalExpanding(1);
	folderview.setVerticalExpanding(1);
	folderview.setFolder(maps_path);
	maincontainer.addWidget(&folderview);
	// Buttons container
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

