#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"
#include "vectorcontainer.h"
#include "button.h"
#include "label.h"
#include "textinput.h"
#include "folderview.h"
#include "callback.h"

#include <string>

namespace Hpp
{

namespace Gui
{

class Filedialog : public Window
{

public:

	enum Type { SAVE, OPEN };

	static uint32_t const SUBMIT = 0;
	static uint32_t const CANCEL = 1;

	inline Filedialog(void);
	inline virtual ~Filedialog(void);

	inline void setType(Type type);
	inline void setFileextension(std::string const& fileext) { this->fileext = fileext; }
	inline void setSelectMultiple(bool select_multiple = true) { folderview.setSelectMultiple(select_multiple); }

	inline void setCallbackFunc(CallbackFuncWithType callback, void* data);

	inline Path getPath(void) const;

private:

	// Options
	Type type;
	std::string fileext;

	CallbackFuncWithType callback;
	void* callback_data;

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

	// Callback functions
	inline static void guiCallback(Widget* widget, void* filedialog_raw);
	inline static void guiCallbackWithType(Widget* widget, uint32_t type, void* filedialog_raw);

};

inline Filedialog::Filedialog(void) :
type(SAVE),
callback(NULL)
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
	pathinput.setCallbackFunc(guiCallback, this);
	filenameinput.setCallbackFunc(guiCallback, this);
	newfolderbutton.setCallbackFunc(guiCallback, this);
	cancelbutton.setCallbackFunc(guiCallback, this);
	saveloadbutton.setCallbackFunc(guiCallback, this);
	folderview.setCallbackFunc(guiCallbackWithType, this);

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

inline void Filedialog::setCallbackFunc(CallbackFuncWithType callback, void* data)
{
	this->callback = callback;
	callback_data = data;
}

inline Path Filedialog::getPath(void) const
{
	return folderview.getFolder() / filenameinput.getValue().stl_string();
}

inline void Filedialog::guiCallback(Widget* widget, void* filedialog_raw)
{
	Filedialog* filedialog = reinterpret_cast< Filedialog* >(filedialog_raw);
	if (widget == &filedialog->cancelbutton) {
		// Do callback if it has been set
		if (filedialog->callback) {
			filedialog->callback(filedialog, Filedialog::CANCEL, filedialog->callback_data);
		}
	} else if (widget == &filedialog->filenameinput || widget == &filedialog->saveloadbutton) {
		// Do callback if it has been set
		if (filedialog->callback) {
			filedialog->callback(filedialog, Filedialog::SUBMIT, filedialog->callback_data);
		}
	} else if (widget == &filedialog->pathinput) {
		UnicodeString old_value = filedialog->folderview.getFolder().toString(true);
		try {
			Path new_path(filedialog->pathinput.getValue().stl_string());
			filedialog->folderview.setFolder(new_path);
		}
		catch ( ... ) {
			filedialog->pathinput.setValue(old_value);
		}
	}
}

inline void Filedialog::guiCallbackWithType(Widget* widget, uint32_t type, void* filedialog_raw)
{
	Filedialog* filedialog = reinterpret_cast< Filedialog* >(filedialog_raw);
	if (widget == &filedialog->folderview) {
		if (type == Folderview::SELECTION_CHANGED) {
			Folderview::SelectedItems items_sel = filedialog->folderview.getSelectedItems();
			if (!items_sel.empty()) {
				size_t item_id = *items_sel.begin();
				Path::DirChild item = filedialog->folderview.getItem(item_id);
				if (item.type == Path::FILE) {
					filedialog->filenameinput.setValue(item.name);
				}
			}
		} else if (type == Folderview::DOUBLE_CLICKED) {
HppAssert(false, "Not implemented yet!");
		}
	}
}

}

}

#endif

