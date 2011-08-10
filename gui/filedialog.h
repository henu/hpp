#ifndef HPP_GUI_FILEDIALOG_H
#define HPP_GUI_FILEDIALOG_H

#include "window.h"

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

};

inline Filedialog::Filedialog(void) :
type(SAVE),
selectmultiple(false)
{
}

inline Filedialog::~Filedialog(void)
{
}

}

}

#endif

