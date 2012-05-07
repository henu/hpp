#ifndef HPP_GUI_FOLDERVIEWCONTENTS_H
#define HPP_GUI_FOLDERVIEWCONTENTS_H

#include "widget.h"
#include "renderer.h"

#include "../path.h"
#include "../unicodestring.h"

namespace Hpp
{

namespace Gui
{

class Folderview;

class FolderviewContents : public Widget
{

public:

	typedef std::set< size_t > SelectedItems;

	inline FolderviewContents(Folderview* folderview);
	inline virtual ~FolderviewContents(void);

	inline void setSelectMultiple(bool select_multiple = true) { this->select_multiple = select_multiple; }
	inline bool getSelectMultiple(void) const { return select_multiple; }

	inline void setFolder(Path const& path);
	inline Path getFolder(void) const;

	inline SelectedItems getSelectedItems(void) const { return items_sel; }
	inline Path::DirChild getItem(size_t item_id) const;

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	Folderview* folderview;

	bool select_multiple;

	Path path;
	Path::DirChildren items;

	SelectedItems items_sel;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);

	inline void reloadFolderContents(void);
	void scrollContents(Real amount);
	void selectionChanged(void);

};

inline FolderviewContents::FolderviewContents(Folderview* folderview) :
folderview(folderview),
select_multiple(true)
{
}

inline FolderviewContents::~FolderviewContents(void)
{
}

inline void FolderviewContents::setFolder(Path const& path)
{
	Path old_path = this->path;
	this->path = path;
	try {
		reloadFolderContents();
	}
	catch ( ... ) {
		this->path = old_path;
		throw;
	}
}

inline Path FolderviewContents::getFolder(void) const
{
	return path;
}

inline Path::DirChild FolderviewContents::getItem(size_t item_id) const
{
	if (item_id >= items.size()) {
		throw Hpp::Exception("Unable to get item because item ID is too big!");
	}
	return items[item_id];
}

inline uint32_t FolderviewContents::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) {
		return 0;
	}
	uint32_t min_width = 0;
	for (Path::DirChildren::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		Path::DirChild const& item = *items_it;
		UnicodeString item_name = item.name;
		min_width = std::max(min_width, rend->getMinimumFolderviewContentsWidth(item_name));
	}
	return min_width;
}

inline uint32_t FolderviewContents::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) {
		return 0;
	}
	return rend->getFolderviewContentsHeight(items.size());
}

inline void FolderviewContents::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) {
		return;
	}
	rend->renderFolderviewContents(x_origin, y_origin, this, items);
}

inline bool FolderviewContents::onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key)
{
	(void)mouse_x;
	(void)mouse_y;
	if (mouse_key == Mousekey::WHEEL_UP) {
		scrollContents(-2.0);
	} else if (mouse_key == Mousekey::WHEEL_DOWN) {
		scrollContents(2.0);
	} else if (mouse_key == Mousekey::LEFT) {
		HppAssert(mouse_y >= 0, "Mouse is really outside of FolderviewContents!");
		Renderer* rend = getRenderer();
		if (rend) {
			size_t item_id = mouse_y / rend->getFolderviewContentsHeight(1);
// TODO: Check if keys are pressed!
			bool shift_pressed = false;
			bool ctrl_pressed = false;
			if (!select_multiple || (!shift_pressed && !ctrl_pressed)) {
				items_sel.clear();
				if (item_id < items.size()) {
					items_sel.insert(item_id);
				}
			} else if (select_multiple && shift_pressed) {
// TODO: Code this!
HppAssert(false, "Not implemented yet!");
			} else if (select_multiple && ctrl_pressed) {
				if (items_sel.find(item_id) == items_sel.end()) {
					items_sel.insert(item_id);
				} else {
					items_sel.erase(item_id);
				}
			}
			selectionChanged();
		}
	}
	return true;
}

inline void FolderviewContents::reloadFolderContents(void)
{
	Path::DirChildren new_items;
	path.listDir(new_items, true);
	items.swap(new_items);
	markSizeChanged();
}

}

}

#endif
