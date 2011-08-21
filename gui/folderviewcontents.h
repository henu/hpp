#ifndef HPP_GUI_FOLDERVIEWCONTENTS_H
#define HPP_GUI_FOLDERVIEWCONTENTS_H

#include "widget.h"

#include "../path.h"

namespace Hpp
{

namespace Gui
{

class FolderviewContents : public Widget
{

public:

	inline FolderviewContents(void);
	inline virtual ~FolderviewContents(void);

	inline void setFolder(Path const& path);

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	Path path;
	FolderChildren items;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);

	inline void reloadFolderContents(void);

};

inline FolderviewContents::FolderviewContents(void)
{
}

inline FolderviewContents::~FolderviewContents(void)
{
}

inline void FolderviewContents::setFolder(Path const& path)
{
	this->path = path;
	reloadFolderContents();
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

inline void FolderviewContents::reloadFolderContents(void)
{
	listFolderChildren(items, path);
	markSizeChanged();
}

}

}

#endif
