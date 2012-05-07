#ifndef HPP_GUI_FOLDERVIEW_H
#define HPP_GUI_FOLDERVIEW_H

#include "scrollbox.h"
#include "containerwidget.h"
#include "renderer.h"
#include "folderviewcontents.h"
#include "callback.h"

#include "../unicodestring.h"
#include "../path.h"

namespace Hpp
{

namespace Gui
{

class Folderview : public Containerwidget
{

	friend class FolderviewContents;

public:

	typedef std::set< size_t > SelectedItems;

	static uint32_t const SELECTION_CHANGED = 0;
	static uint32_t const DOUBLE_CLICKED = 1;

	inline Folderview(void);
	inline virtual ~Folderview(void);

	inline void setSelectMultiple(bool select_multiple = true) { contents.setSelectMultiple(select_multiple); }
	inline bool getSelectMultiple(void) const { return contents.getSelectMultiple(); }

	inline void setFolder(Path const& path);
	inline Path getFolder(void) const;

	inline SelectedItems getSelectedItems(void) const { return contents.getSelectedItems(); }
	inline Path::DirChild getItem(size_t item_id) const { return contents.getItem(item_id); }

	inline void setCallbackFunc(CallbackFuncWithType callback, void* data);

	// Virtual functions for Widget
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	// Called by friend class FolderviewContents
	inline void scrollContents(Real amount);
	inline void selectionChanged(void);

private:

	Scrollbox scrollbox;
	FolderviewContents contents;

	CallbackFuncWithType callback;
	void* callback_data;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onSizeChange(void);
	inline virtual void onEnvironmentUpdated(void);

	inline void updateScrollboxSize(void);

};

inline Folderview::Folderview(void) :
contents(this),
callback(NULL)
{
	addChild(&scrollbox);
	scrollbox.setHorizontalScrollbar(Scrollbox::ON_DEMAND);
	scrollbox.setVerticalScrollbar(Scrollbox::ALWAYS);
	scrollbox.setContent(&contents);
}

inline Folderview::~Folderview(void)
{
}

inline void Folderview::setFolder(Path const& path)
{
	contents.setFolder(path);
}

inline Path Folderview::getFolder(void) const
{
	return contents.getFolder();
}

inline void Folderview::setCallbackFunc(CallbackFuncWithType callback, void* data)
{
	this->callback = callback;
	callback_data = data;
}

inline uint32_t Folderview::getMinWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMinimumFolderviewWidth();
}

inline uint32_t Folderview::getMinHeight(uint32_t width) const
{
	(void)width;
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getFolderviewHeight();
}

inline void Folderview::scrollContents(Real amount)
{
	scrollbox.scrollVerticallyAsButtons(amount);
}

inline void Folderview::selectionChanged(void)
{
	if (callback) {
		callback(this, SELECTION_CHANGED, callback_data);
	}
}

inline void Folderview::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	rend->renderFolderview(x_origin, y_origin, this);
}

inline void Folderview::onSizeChange(void)
{
	updateScrollboxSize();
}

inline void Folderview::onEnvironmentUpdated(void)
{
	Renderer* rend = getRenderer();
	if (rend) {
		uint32_t move_amount = rend->getFolderviewContentsHeight(3);
		scrollbox.setHorizontalScrollbarButtonmove(move_amount);
		scrollbox.setVerticalScrollbarButtonmove(move_amount);
	}
	markSizeChanged();
}

inline void Folderview::updateScrollboxSize(void)
{
	Renderer const* rend = getRenderer();
	if (!rend) return;
	// Get edge sizes
	uint32_t edge_top, edge_left, edge_right, edge_bottom;
	rend->getFolderviewEdgeSizes(edge_top, edge_left, edge_right, edge_bottom);
	setChildPosition(&scrollbox, edge_left, edge_top);
	setChildSize(&scrollbox, getWidth() - edge_left - edge_right, getHeight() - edge_top - edge_bottom);
}

}

}

#endif

