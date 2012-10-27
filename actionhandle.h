#ifndef HPP_ACTIONHANDLE_H
#define HPP_ACTIONHANDLE_H

#include "time.h"
#include "action.h"

namespace Hpp
{

class Entity;

class Actionhandle
{

	friend class Entity;

public:

	~Actionhandle(void);

	void addTime(Delay const& d);

	inline void setWeight(Real weight) { this->weight = weight; }

private:

	// Actionhandles may be only created by entities
	inline Actionhandle(Entity* ent, Action const* act);

	Entity* ent;
	Action const* act;

	float time;
	Real weight;

};

inline Actionhandle::Actionhandle(Entity* ent, Action const* act) :
ent(ent),
act(act),
time(0.0),
weight(1.0)
{
}

}

#endif
