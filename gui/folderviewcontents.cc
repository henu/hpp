#include "folderviewcontents.h"

#include "folderview.h"

namespace Hpp
{

namespace Gui
{

void FolderviewContents::scrollContents(Real amount)
{
	folderview->scrollContents(amount);
}

void FolderviewContents::selectionChanged(void)
{
	folderview->selectionChanged();
}

}

}
