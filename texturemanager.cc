#include "texturemanager.h"

#include "texture.h"

#include "exception.h"

namespace Hpp
{

Texturemanager Texturemanager::instance;

Texturemanager::Texturemanager(void)
{
}

Texturemanager::~Texturemanager(void)
{
	Lock rtexs_lock(rtexs_mutex);
	HppAssert(rtexs.empty(), "Releasable textures have not been released!");
}

}
