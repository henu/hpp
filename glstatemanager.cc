#include "glstatemanager.h"

#include "bufferobject.h"
#include "glsystem.h"
#include "exception.h"
#include "cast.h"

namespace Hpp
{

GlStatemanager GlStatemanager::instance;

void GlStatemanager::setVertexattributeState(GLuint vertexattr_loc, Bufferobject const* bo)
{
	while (vertexattr_loc >= instance.vattrs.size()) {
		instance.vattrs.push_back(Vertexattribute());
	}
	instance.vattrs[vertexattr_loc].bo_should_be = bo;
}

void GlStatemanager::clearAllVertexarrays(void)
{
	for (Vertexattributes::iterator vattrs_it = instance.vattrs.begin();
	     vattrs_it != instance.vattrs.end();
	     ++ vattrs_it) {
		Vertexattribute& vattr = *vattrs_it;
		vattr.bo_should_be = NULL;
	}
}

void GlStatemanager::syncVertexarrays(void)
{
	for (GLuint vattr_loc = 0;
	     vattr_loc < instance.vattrs.size();
	     ++ vattr_loc) {
		Vertexattribute& vattr = instance.vattrs[vattr_loc];

		// If there is buffer bound, but there should be
		// none, then disable this vertex attribute
		if (vattr.bo_bound && !vattr.bo_should_be) {
			GlSystem::DisableVertexAttribArray(vattr_loc);
			vattr.bo_bound = NULL;
		}
		// If there is no Bufferobject bound at all but there should be
		else if (!vattr.bo_bound && vattr.bo_should_be) {
			Bufferobject const* bo = vattr.bo_should_be;
			GlSystem::EnableVertexAttribArray(vattr_loc);
			GlSystem::BindBuffer(bo->getTarget(), bo->getBufferId());
			GlSystem::VertexAttribPointer(vattr_loc, bo->getComponents(), bo->getType(), bo->getNormalized(), 0, NULL);
			vattr.bo_bound = bo;
		}
		// If there is wrong Bufferobject bound
		else if (vattr.bo_bound && vattr.bo_bound != vattr.bo_should_be) {
			Bufferobject const* bo = vattr.bo_should_be;
			GlSystem::BindBuffer(bo->getTarget(), bo->getBufferId());
			GlSystem::VertexAttribPointer(vattr_loc, bo->getComponents(), bo->getType(), bo->getNormalized(), 0, NULL);
			vattr.bo_bound = bo;
		}
	}
}

void GlStatemanager::markBufferobjectDestroyed(Bufferobject const* bo)
{
	for (GLuint vattr_loc = 0;
	     vattr_loc < instance.vattrs.size();
	     ++ vattr_loc) {
		Vertexattribute& vattr = instance.vattrs[vattr_loc];
		// If Bufferobject is currently bound here, then unbound it
		if (vattr.bo_bound == bo) {
			GlSystem::DisableVertexAttribArray(vattr_loc);
			vattr.bo_bound = NULL;
		}
		if (vattr.bo_should_be == bo) {
			throw Exception("Bufferobject is being destroyed but at the same time it should be bound to vertex attribute #" + Hpp::sizeToStr(vattr_loc) + "!");
		}
	}
}

}
