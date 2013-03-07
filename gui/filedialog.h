#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"
#include "vectorcontainer.h"
#include "button.h"
#include "label.h"
#include "textinput.h"
#include "folderview.h"
#include "eventlistener.h"

#include <string>

namespace Hpp
{

namespace Gui
{

class Filedialog : public Window, public Eventlistener
{

public:

	enum Type { SAVE, OPEN };

	static int const SUBMIT = 0;
	static int const CANCEL = 1;

	inline Filedialog(void);
	inline virtual ~Filedialog(void);

	inline void setType(Type type);
	inline void setFileextension(std::string const& fileext) { this->fileext = fileext; }
	inline void setSelectMultiple(bool select_multiple = true) { folderview.setSelectMultiple(select_multiple); }

	inline Path getPath(void) const;

private:

	// Options
	Type type;
	std::string fileext;

	// Widgets
	Vectorcontainer maincontainer;
	Vectorcontainer filenamecontainer;
	Vectorcontainer pathfoldercontainer;
	Vectorcontainer buttonscontainer;
	Label filenamelabel;
	Textinput filenameinput;
	Textinput pathinput;
	Button newfolderbutton;
	Folderview folderview;
	Button cancelbutton;
	Button saveloadbutton;

	inline virtual bool handleGuiEvent(GuiEvent const& event);

};

inline Filedialog::Filedialog(void) :
type(SAVE)
{
	Path maps_path = Path::getConfig() / "hme_mapeditor" / "levels";
	maps_path.ensureDirExists();

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
	saveloadbutton.setLabel("Save");
	buttonscontainer.addWidget(&cancelbutton);
	buttonscontainer.addWidget(&saveloadbutton);

	// Set callbacks
	pathinput.setEventlistener(this);
	filenameinput.setEventlistener(this);
	newfolderbutton.setEventlistener(this);
	cancelbutton.setEventlistener(this);
	saveloadbutton.setEventlistener(this);
	folderview.setEventlistener(this);

}

inline Filedialog::~Filedialog(void)
{
}

inline void Filedialog::setType(Type type)
{
	this->type = type;
	if (type == Filedialog::SAVE) {
		saveloadbutton.setLabel("Save");
	} else {
		saveloadbutton.setLabel("Open");
	}
}

inline Path Filedialog::getPath(void) const
{
	return folderview.getFolder() / filenameinput.getValue().stl_string();
}

inline bool Filedialog::handleGuiEvent(GuiEvent const& event)
{
	if (event.getWidget() == &cancelbutton) {
		fireEvent(Filedialog::CANCEL);
	} else if (event.getWidget() == &filenameinput || event.getWidget() == &saveloadbutton) {
		fireEvent(Filedialog::SUBMIT);
	} else if (event.getWidget() == &pathinput) {
		UnicodeString old_value = folderview.getFolder().toString(true);
		try {
			Path new_path(pathinput.getValue().stl_string());
			folderview.setFolder(new_path);
		}
		catch ( ... ) {
			pathinput.setValue(old_value);
		}
	} else if (event.getWidget() == &folderview) {
		if (event.getAction() == Folderview::SELECTION_CHANGED) {
			Folderview::SelectedItems items_sel = folderview.getSelectedItems();
			if (!items_sel.empty()) {
				size_t item_id = *items_sel.begin();
				Path::DirChild item = folderview.getItem(item_id);
				if (item.type == Path::FILE) {
					filenameinput.setValue(item.name);
				}
			}
		} else if (event.getAction() == Folderview::DOUBLE_CLICKED) {
HppAssert(false, "Not implemented yet!");
		}
	}
	return false;
}

}

}

#endif

