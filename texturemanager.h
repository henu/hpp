#ifndef HPP_TEXTUREMANAGER_H
#define HPP_TEXTUREMANAGER_H

#include "mutex.h"
#include "lock.h"
#include "gldebug.h"

#include <map>
#include <vector>
#include <string>

namespace Hpp
{

// Classes that doesn't need their headers included for now
class Texture;

class Texturemanager
{

	friend class Display;
	friend class Texture;

public:

	// Checks if material exists
	static bool textureExists(std::string const& name);

	// Adds/deletes materials. Note, that if material already exists while
	// adding new, the old one will be deleted.
	static void addTexture(std::string const& name, Texture* texture);
	static void deleteTexture(std::string const& name);

	// Clears the whole manager
	static void clear(void);

	// Gets specific texture
	static Texture* getTexture(std::string const& name);

private:

	inline static void addReleasableTexture(GLuint gl_texture);
	inline static void cleanReleasableTextures(void);

private:

	// ----------------------------------------
	// Private types
	// ----------------------------------------

	typedef std::vector< GLuint > ReleasableTextures;


	// ----------------------------------------
	// Data
	// ----------------------------------------

	// The one and only instance of this class
	static Texturemanager instance;

	// Releasable textures and mutex for them
	ReleasableTextures rtexs;
	Mutex rtexs_mutex;


	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// Constructor and destructor in private to prevent other instances.
	Texturemanager(void);
	~Texturemanager(void);

};

inline void Texturemanager::addReleasableTexture(GLuint gl_texture)
{
	Lock lock(instance.rtexs_mutex);
	instance.rtexs.push_back(gl_texture);
}

inline void Texturemanager::cleanReleasableTextures(void)
{
	HppCheckForCorrectThread();
	// Release textures
	Lock rtexs_lock(instance.rtexs_mutex);
	if (!instance.rtexs.empty()) {
		glDeleteTextures(instance.rtexs.size(), &instance.rtexs[0]);
		instance.rtexs.clear();
	}
}

}

#endif
