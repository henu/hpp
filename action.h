#ifndef HPP_ACTION_H
#define HPP_ACTION_H

#include "real.h"

#include <map>
#include <string>

namespace Hpp
{

struct Action
{
	// Types
	struct Channelpoint {
		Real value;
		Real weight;
	};
	typedef std::map< float, Channelpoint > Channel;
	struct Bonecontrol
	{
		Channel loc_x;
		Channel loc_y;
		Channel loc_z;
		Channel quat_x;
		Channel quat_y;
		Channel quat_z;
		Channel quat_w;
		Channel scale_x;
		Channel scale_y;
		Channel scale_z;
	};
	typedef std::map< std::string, Bonecontrol > Bonecontrols;

	// Data
	float begin;
	float end;
	// Bones that will be altered.
	Bonecontrols bones;
};
typedef std::map< std::string, Action > Actions;

}

#endif
