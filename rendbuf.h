#ifndef HPP_RENDBUF_H
#define HPP_RENDBUF_H

#include "vbomanager.h"
#include "glsystem.h"
#include "color.h"
#include "vector2.h"
#include "vector3.h"
#include "assert.h"
#include "display.h"
#include "memwatch.h"
#include "rendbuf_enums.h"

#include "inc_gl.h"
#include <cstring>

namespace Hpp
{

template< class Type >
class Rendbuf
{

public:

	// Constructors and destructor
	inline Rendbuf(bool autorelease = false) :
	autorelease(autorelease),
	res(0),
	items(0),
	vbo(RendbufEnums::NO_VBO)
	{
		#ifndef NDEBUG
		source_releases_automatically = false;
		#endif
	}
	inline Rendbuf(Rendbuf< Type > const& autob) :
	autorelease(false),
	buf(autob.buf),
	res(autob.res),
	items(autob.items),
	vbo(autob.vbo),
	vbo_id(autob.vbo_id)
	{
		// If the source Rendbuf is released automatically, then it is
		// good thing if this Rendbuf knows it.
		#ifndef NDEBUG
		if (autob.autorelease) {
			source_releases_automatically = true;
		} else {
			source_releases_automatically = false;
		}
		#endif
		// If vbo is not yet got, then ask it now
		if (vbo == RendbufEnums::VBO_NOT_READY) {
			VboManager::registerVBONeeder(buf, &vbo_id, &vbo);
		}
	}
	inline ~Rendbuf(void) {
		if (autorelease && vbo != RendbufEnums::NO_VBO) {
			if (!Display::isThisRenderingThread() ||
			    vbo == RendbufEnums::VBO_NOT_READY ||
			    vbo == RendbufEnums::VBO_READY_FROM_MANAGER) {
				// First VBO id is asked from Display and not
				// got, then we need to unregister ourself from
				// there.
				if (vbo == RendbufEnums::VBO_NOT_READY) {
					VboManager::unregisterVBONeeder(buf, &vbo_id);
				}
				// Ask display to release VBO. If it knows
				// about this VBO, it can find it by using
				// buffer. If it does now about it, then it
				// will release it using ID value. The latter
				// case may happen if Rendbuf is converted to
				// VBO in OpenGL thread, but is released in
				// another thread.
				VboManager::releaseVBO(buf, vbo_id);
			} else {
				GlSystem::DeleteBuffers(1, &vbo_id);
			}
		} else if (vbo == RendbufEnums::VBO_NOT_READY) {
			VboManager::unregisterVBONeeder(buf, &vbo_id);
		}
		if (autorelease && res > 0) {
			HppStopWatchingMem(buf);
			delete[] buf;
		}
	}

	// Copy constructor. Note that Rendbufs with automatic release
	// cannot be copied.
	Rendbuf const& operator=(Rendbuf< Type > const& autob)
	{
		if (autorelease && vbo != RendbufEnums::NO_VBO) {
			if (!Display::isThisRenderingThread() ||
			    vbo == RendbufEnums::VBO_NOT_READY ||
			    vbo == RendbufEnums::VBO_READY_FROM_MANAGER) {
				// First VBO id is asked from Display and not
				// got, then we need to unregister ourself from
				// there.
				if (vbo == RendbufEnums::VBO_NOT_READY) {
					VboManager::unregisterVBONeeder(buf, &vbo_id);
				}
				// Ask display to release VBO. If it knows
				// about this VBO, it can find it by using
				// buffer. If it does now about it, then it
				// will release it using ID value. The latter
				// case may happen if Rendbuf is converted to
				// VBO in OpenGL thread, but is released in
				// another thread.
				VboManager::releaseVBO(buf, vbo_id);
			} else {
				GlSystem::DeleteBuffers(1, &vbo_id);
			}
		} else if (vbo == RendbufEnums::VBO_NOT_READY) {
			VboManager::unregisterVBONeeder(buf, &vbo_id);
		}
		if (autorelease && res > 0) {
			HppStopWatchingMem(buf);
			delete[] buf;
		}
		// If the source Rendbuf is released automatically, then it is
		// good thing if this Rendbuf knows it.
		#ifndef NDEBUG
		if (autob.autorelease) {
			source_releases_automatically = true;
		}
		#endif
		autorelease = false;
		buf = autob.buf;
		res = autob.res;
		items = autob.items;
		vbo = autob.vbo;
		vbo_id = autob.vbo_id;
		// If vbo is not yet got, then ask it now
		if (vbo == RendbufEnums::VBO_NOT_READY) {
			VboManager::registerVBONeeder(buf, &vbo_id, &vbo);
		}
		return *this;
	}

	// Checks if this buffer releases itself when it is destroyed.
	inline bool releasedAutomatically(void) const { return autorelease; }

	// Releases data. This MUST be called before deleting last instance of
	// Rendbuf if autorelease is not toggled off. This may not be called if
	// autorelease is turned on
	inline void release(void)
	{
		HppAssert(!autorelease, "Unable to release data of Rendbuf that is set to automatic release!");
		// Ensure we are not trying to release data that some other
		// Rendbuf is going to release.
		#ifndef NDEBUG
		HppAssert(!source_releases_automatically, "Unable to release data of Rendbuf because some other Rendbuf wants to release it automatically!");
		#endif
		if (vbo != RendbufEnums::NO_VBO) {
			if (!Display::isThisRenderingThread() ||
			    vbo == RendbufEnums::VBO_NOT_READY ||
			    vbo == RendbufEnums::VBO_READY_FROM_MANAGER) {
				if (vbo == RendbufEnums::VBO_NOT_READY) {
					VboManager::unregisterVBONeeder(buf, &vbo_id);
				}
				// Ask display to release VBO. If it knows
				// about this VBO, it can find it by using
				// buffer. If it does now about it, then it
				// will release it using ID value. The latter
				// case may happen if Rendbuf is converted to
				// VBO in OpenGL thread, but is released in
				// another thread.
				VboManager::releaseVBO(buf, vbo_id);
			} else {
				GlSystem::DeleteBuffers(1, &vbo_id);
			}
			vbo = RendbufEnums::NO_VBO;
		}
		if (res > 0) {
			HppStopWatchingMem(buf);
			delete[] buf;
			res = 0;
			items = 0;
		}
	}

	// Reservers specific amount of items to the memory
	inline void reserve(size_t res) {
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		// Ensure we are not trying to release data that some other
		// Rendbuf is going to release.
		#ifndef NDEBUG
		HppAssert(!source_releases_automatically, "Unable to reserve data for Rendbuft that has its sourece set to release it!");
		#endif
		if (this->res < res) {
			if (this->res > 0) {
				Type* new_buf = new Type[res];
				memcpy(new_buf, buf, items * sizeof(Type));
				HppStopWatchingMem(buf);
				delete[] buf;
				buf = new_buf;
			} else {
				buf = new Type[res];
			}
			HppStartWatchingMem(buf);
			this->res = res;
		}
	}

	// Same as reserve, but in multiples of 2, 3 and 4.
	inline void reserve2(size_t res) { reserve(res * 2); }
	inline void reserve3(size_t res) { reserve(res * 3); }
	inline void reserve4(size_t res) { reserve(res * 4); }

	// Allocates specific amount of unitialized items to container.
	inline void insertUninitializedData(size_t size) {
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + size);
		items += size;
	}

	// Returns if Rendbuf is empty
	inline bool empty(void) const { return items == 0; }

	// Clears container
	inline void clear(void)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		items = 0;
	}

	// Get's amount of items in Rendbuf.
	inline size_t size(void) const
	{
		return items;
	}
	inline size_t size2(void) const
	{
		HppAssert(items % 2 == 0, "This is not a two item Rendbuf!");
		return items >> 1;
	}
	inline size_t size3(void) const
	{
		HppAssert(items % 3 == 0, "This is not a three item Rendbuf!");
		return items / 3;
	}
	inline size_t size4(void) const
	{
		HppAssert(items % 4 == 0, "This is not a four item Rendbuf!");
		return items >> 2;
	}

	// Operator[]
	inline Type& operator[](size_t offset)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		HppAssert(offset < items, "Out of range!");
		return buf[offset];
	}
	inline Type operator[](size_t offset) const
	{
		HppAssert(offset < items, "Out of range!");
		return buf[offset];
	}

	// Push back operators
	inline void push_back(Type var)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 1);
		buf[items ++] = var;
	}
	inline void push_back2(Type var0, Type var1)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 2);
		buf[items ++] = var0;
		buf[items ++] = var1;
	}
	inline void push_back3(Type var0, Type var1, Type var2)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 3);
		buf[items ++] = var0;
		buf[items ++] = var1;
		buf[items ++] = var2;
	}
	inline void push_back4(Type var0, Type var1, Type var2, Type var3)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 4);
		buf[items ++] = var0;
		buf[items ++] = var1;
		buf[items ++] = var2;
		buf[items ++] = var3;
	}
	inline void push_back_v2(Vector2 const& vect)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 2);
		buf[items ++] = vect.x;
		buf[items ++] = vect.y;
	}
	inline void push_back_v3(Vector3 const& vect)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 3);
		buf[items ++] = vect.x;
		buf[items ++] = vect.y;
		buf[items ++] = vect.z;
	}

	inline void push_back_c(Color const& c)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		ensureSpace(items + 3);
		buf[items ++] = c.getRed();
		buf[items ++] = c.getGreen();
		buf[items ++] = c.getBlue();
	}

	// Insert operators
	template< typename Iter1, typename Iter2 >
	inline void insert(Iter1 begin, Iter2 end)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
// TODO/Optimize: Use random access iterators if possible!
		for (Iter1 it = begin; it != end; it ++) {
			push_back(*it);
		}
	}

	// Gets C-array version of Rendbuf
	inline Type* c_buf(void)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
		return buf;
	}
	inline Type const* c_buf(void) const { return buf; }

	// Converts this Rendbuf to VBO. After this, write operations are not
	// allowed.
	inline void convertToVBO(RendbufEnums::VBOType type)
	{
		if (items > 0) {
			HppAssert(vbo == RendbufEnums::NO_VBO, "Rendbuffer is already converted to VBO!");
			if (Display::isThisRenderingThread()) {
				GlSystem::GenBuffers(1, &vbo_id);
				GlSystem::BindBuffer(type, vbo_id);
				GlSystem::BufferData(type, sizeof(Type) * items, buf, GL_STATIC_DRAW_ARB);
				GlSystem::BindBuffer(type, 0);
				vbo = RendbufEnums::VBO_READY;
			} else {
				// Because we are in wrong thread, we need to
				// ask for a manager class to do the loading
				// job for us.
				VboManager::addVBOJob(type, buf, sizeof(Type) * items);
				vbo = RendbufEnums::VBO_NOT_READY;
				// We also need to register this class as a
				// needer of vbo_id
				vbo_id = 0;
				VboManager::registerVBONeeder(buf, &vbo_id, &vbo);
			}
		}
	}

	// Use buffer as data or to draw
	inline void use(RendbufEnums::Datatype type, ssize_t elem_size = -1) const
	{
		HppEnsureWatchingMem(buf);
		if (vbo == RendbufEnums::NO_VBO) {
			switch (type) {
			case RendbufEnums::VERTEX:
				if (elem_size < 0) {
					#ifdef HPP_GL_DOUBLE_MATH
					glVertexPointer(3, GL_DOUBLE, sizeof(Type[3]), buf);
					#else
					glVertexPointer(3, GL_FLOAT, sizeof(Type[3]), buf);
					#endif
				} else {
					#ifdef HPP_GL_DOUBLE_MATH
					glVertexPointer(elem_size, GL_DOUBLE, sizeof(Type) * elem_size, buf);
					#else
					glVertexPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, buf);
					#endif
				}
				break;
			case RendbufEnums::NORMAL:
				#ifdef HPP_GL_DOUBLE_MATH
				glNormalPointer(GL_DOUBLE, sizeof(Type[3]), buf);
				#else
				glNormalPointer(GL_FLOAT, sizeof(Type[3]), buf);
				#endif
				break;
			case RendbufEnums::COLOR:
				if (elem_size < 0) {
					glColorPointer(4, GL_FLOAT, sizeof(Type[4]), buf);
				} else {
					glColorPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, buf);
				}
				break;
			case RendbufEnums::TEXCOORD:
				if (elem_size < 0) {
					#ifdef HPP_GL_DOUBLE_MATH
					glTexCoordPointer(2, GL_DOUBLE, sizeof(Type[2]), buf);
					#else
					glTexCoordPointer(2, GL_FLOAT, sizeof(Type[2]), buf);
					#endif
				} else {
					#ifdef HPP_GL_DOUBLE_MATH
					glTexCoordPointer(elem_size, GL_DOUBLE, sizeof(Type) * elem_size, buf);
					#else
					glTexCoordPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, buf);
					#endif
				}
				break;
			}
		} else {
			#ifndef ELIBGL_NOGRAPHICS
			if (vbo == RendbufEnums::VBO_NOT_READY) {
				vbo_id = VboManager::getVBO(buf, &vbo_id);
				vbo = RendbufEnums::VBO_READY_FROM_MANAGER;
			}
			GlSystem::BindBuffer(GL_ARRAY_BUFFER_ARB, vbo_id);
			switch (type) {
			case RendbufEnums::VERTEX:
				if (elem_size < 0) {
					#ifdef HPP_GL_DOUBLE_MATH
					glVertexPointer(3, GL_DOUBLE, sizeof(Type[3]), 0);
					#else
					glVertexPointer(3, GL_FLOAT, sizeof(Type[3]), 0);
					#endif
				} else {
					#ifdef HPP_GL_DOUBLE_MATH
					glVertexPointer(elem_size, GL_DOUBLE, sizeof(Type) * elem_size, 0);
					#else
					glVertexPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, 0);
					#endif
				}
				break;
			case RendbufEnums::NORMAL:
				#ifdef HPP_GL_DOUBLE_MATH
				glNormalPointer(GL_DOUBLE, sizeof(Type[3]), 0);
				#else
				glNormalPointer(GL_FLOAT, sizeof(Type[3]), 0);
				#endif
				break;
			case RendbufEnums::COLOR:
				if (elem_size < 0) {
					glColorPointer(4, GL_FLOAT, sizeof(Type[4]), 0);
				} else {
					glColorPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, 0);
				}
				break;
			case RendbufEnums::TEXCOORD:
				if (elem_size < 0) {
					#ifdef HPP_GL_DOUBLE_MATH
					glTexCoordPointer(2, GL_DOUBLE, sizeof(Type[2]), 0);
					#else
					glTexCoordPointer(2, GL_FLOAT, sizeof(Type[2]), 0);
					#endif
				} else {
					#ifdef HPP_GL_DOUBLE_MATH
					glTexCoordPointer(elem_size, GL_DOUBLE, sizeof(Type) * elem_size, 0);
					#else
					glTexCoordPointer(elem_size, GL_FLOAT, sizeof(Type) * elem_size, 0);
					#endif
				}
				break;
			}
			GlSystem::BindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			#endif
		}
	}
	inline void draw(RendbufEnums::Drawtype type) const
	{
		HppEnsureWatchingMem(buf);
		if (vbo == RendbufEnums::NO_VBO) {
			switch (type) {
			case RendbufEnums::TRIANGLES:
				glDrawElements(GL_TRIANGLES, items, GL_UNSIGNED_INT, buf);
				break;
			case RendbufEnums::QUADS:
				glDrawElements(GL_QUADS, items, GL_UNSIGNED_INT, buf);
				break;
			}
		} else {
			#ifndef ELIBGL_NOGRAPHICS
			if (vbo == RendbufEnums::VBO_NOT_READY) {
				vbo_id = VboManager::getVBO(buf, &vbo_id);
				vbo = RendbufEnums::VBO_READY_FROM_MANAGER;
			}
			GlSystem::BindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_id);
			switch (type) {
			case RendbufEnums::TRIANGLES:
				glDrawElements(GL_TRIANGLES, items, GL_UNSIGNED_INT, 0);
				break;
			case RendbufEnums::QUADS:
				glDrawElements(GL_QUADS, items, GL_UNSIGNED_INT, 0);
				break;
			}
			// Turn off VBO
			GlSystem::BindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
			#endif
		}
	}

private:

	// Possible autorelease. This means that memory will be released
	// automatically.
	bool autorelease;

	// The buffer, it's length and amount items stored to it.
	Type* buf;
	size_t res;
	size_t items;

	// VBO stuff
	mutable RendbufEnums::VBOState vbo;
	mutable GLuint vbo_id;

	// This boolean is used to check if current data is from Rendbuf that
	// autoreleases it's data. This is only used when debugging.
	bool source_releases_automatically;

	// ----------------------------------------
	// Private functions
	// ----------------------------------------

	// Ensures that there is enough space for specific amount of items
	inline void ensureSpace(size_t space)
	{
		HppAssert(vbo == RendbufEnums::NO_VBO, "Unable to modify Rendbuf when it is converted to VBO!");
// TODO: Should source_releases_automatically be checked here? Actually this should not be called if there are any other copies left from this Rendbuf. Or at least if they are being used...
		if (res == 0) {
			res = space + 16;
			buf = new Type[res];
			HppStartWatchingMem(buf);
		} else if (res < space) {
			res = space*2;
			Type* new_buf = new Type[res];
			HppStartWatchingMem(new_buf);
			memcpy(new_buf, buf, items * sizeof(Type));
			HppStopWatchingMem(buf);
			delete[] buf;
			buf = new_buf;
		}
	}

};

}

#endif
