#include "texturemanager.h"

#include "texture.h"
#include "datamanagerbase.h"

#include "exception.h"

namespace Hpp
{

Texturemanager Texturemanager::instance;

// Real container of Textures
DatamanagerBase< Texture > textures;

bool Texturemanager::textureExists(std::string const& name)
{
	return textures.itemExists(name);
}

void Texturemanager::addTexture(std::string const& name, Texture* texture)
{
	textures.addItem(name, texture);
}

void Texturemanager::deleteTexture(std::string const& name)
{
	textures.deleteItem(name);
}

void Texturemanager::clear(void)
{
	textures.deleteAllItems();
}

Texture* Texturemanager::getTexture(std::string const& name)
{
	return textures.getItem(name);
}

Texturemanager::Texturemanager(void)
{
}

Texturemanager::~Texturemanager(void)
{
	textures.deleteAllItems();
	Lock rtexs_lock(rtexs_mutex);
	HppAssert(rtexs.empty(), "Releasable textures have not been released!");
}

}
