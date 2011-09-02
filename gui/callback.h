#ifndef HPP_GUI_CALLBACK_H
#define HPP_GUI_CALLBACK_H

#include <stdint.h>

namespace Hpp
{

namespace Gui
{

class Widget;

typedef void (*CallbackFunc)(Widget*, void* data);
typedef void (*CallbackFuncWithType)(Widget*, uint32_t type, void* data);

}

}

#endif

