#ifndef HPP_FRAMEBUF_H
#define HPP_FRAMEBUF_H

#include "gldebug.h"
#include "glsystem.h"

namespace Hpp
{

class Framebuf
{

public:

	inline Framebuf(GLenum target);
	inline ~Framebuf(void);

	inline void bind(void);

	// This does not ensure, that its being bound, it just unbounds "target".
	inline void unbind(void);

private:

	GLuint id;
	GLenum target;

};

inline Framebuf::Framebuf(GLenum target) :
id(0),
target(target)
{
	HppCheckGlErrors();
	Hpp::GlSystem::GenFramebuffers(1, &id);
	HppCheckGlErrors();
}

inline Framebuf::~Framebuf(void)
{
	HppCheckGlErrors();
	Hpp::GlSystem::DeleteFramebuffers(1, &id);
	HppCheckGlErrors();
}

void Framebuf::bind(void)
{
	HppCheckGlErrors();
	Hpp::GlSystem::BindFramebuffer(target, id);
	HppCheckGlErrors();
}

void Framebuf::unbind(void)
{
	HppCheckGlErrors();
	Hpp::GlSystem::BindFramebuffer(target, 0);
	HppCheckGlErrors();
}

}

#endif
