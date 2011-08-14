#ifndef HPP_GUI_FOLDERVIEW_H
#define HPP_GUI_FOLDERVIEW_H

#include "containerwidget.h"
#include "renderer.h"

#include "../unicodestring.h"
#include "../path.h"

namespace Hpp
{

namespace Gui
{

class Folderview : public Containerwidget
{

public:

	inline Folderview(void);
	inline virtual ~Folderview(void);

	inline void setFolder(Path const& path);

	// Virtual functions for Widget
	inline virtual uint32_t getMaxWidth(void) const;
	inline virtual uint32_t getMinWidth(void) const;
	inline virtual uint32_t getMinHeight(uint32_t width) const;

private:

	enum Itemtype { DIR, FOLDER };
	struct Item
	{
		UnicodeString name;
		Itemtype type;
	};
	typedef std::vector< Item > Items;

	Path path;
	Items items;

	// Virtual functions for Widget
	inline virtual void doRendering(int32_t x_origin, int32_t y_origin);
	inline virtual void onEnvironmentUpdated(void);

};

inline Folderview::Folderview(void)
{
}

inline Folderview::~Folderview(void)
{
}

inline void Folderview::setFolder(Path const& path)
{
	this->path = path;
// TODO: Update items!
}

inline uint32_t Folderview::getMaxWidth(void) const
{
	Renderer const* rend = getRenderer();
	if (!rend) return 0;
	return rend->getMinimumFolderviewWidth();
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

inline void Folderview::doRendering(int32_t x_origin, int32_t y_origin)
{
	Renderer* rend = getRenderer();
	if (!rend) return;
	return rend->renderFolderview(x_origin, y_origin, this);
}

inline void Folderview::onEnvironmentUpdated(void)
{
	markSizeChanged();
}

}

}

#endif

