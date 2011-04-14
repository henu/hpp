#include "actionhandle.h"

#include "entity.h"

namespace Hpp
{

Actionhandle::~Actionhandle(void)
{
	ent->unregisterActionhandle(this);
}


void Actionhandle::addTime(Hpp::Delay const& d)
{
	float act_duration = act->end - act->begin;
	if (act_duration <= 0.0) {
		time = act->begin;
	} else {
		time += d.getSecondsAsDouble();
// TODO: Optimize!
		while (time < act->begin) {
			time += act_duration;
		}
		while (time >= act->end) {
			time -= act_duration;
		}
	}
	ent->markSkeletonOutOfDate();
}

}
