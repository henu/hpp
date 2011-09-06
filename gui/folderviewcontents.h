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

	inline FolderviewContents(Folderview* folderview);
	inline virtual ~FolderviewContents(void);

	inline void setFolder(Path const& path);
	inline Path getFolder(void) const;

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	Folderview* folderview;

	Path path;
	FolderChildren items;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual bool onMouseKeyDown(int32_t mouse_x, int32_t mouse_y, Mousekey::Keycode mouse_key);

	inline void reloadFolderContents(void);
	void scrollContents(Real amount);

};

inline FolderviewContents::FolderviewContents(Folderview* folderview) :
folderview(folderview)
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

inline uint32_t FolderviewContents::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) {
		return 0;
	}
	uint32_t min_width = 0;
	for (FolderChildren::const_iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
		FolderChild const& item = *items_it;
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
// TODO: Code this!
HppAssert(false, "Not implemented yet!");
	}
	return true;
}

inline void FolderviewContents::reloadFolderContents(void)
{
	FolderChildren new_items;
	listFolderChildren(new_items, path);
	items.swap(new_items);
	markSizeChanged();
}

}

}

#endif
