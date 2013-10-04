#ifndef HPP_SKELETON_H
#define HPP_SKELETON_H

#include "action.h"
#include "serialize.h"
#include "vector3.h"
#include "matrix3.h"
#include "exception.h"

#include <string>
#include <set>

namespace Hpp
{

class Skeleton
{

	friend class Rawmesh;
	friend class Entity;

public:

private:

	// Functions for friends
	inline void readVersion100(std::istream& strm);
	inline Action const* getAction(std::string const& act_name) const;

private:

	typedef std::set< std::string > Nameset;

	struct Bone
	{
		Vector3 pos_rel;
		Vector3 pos_abs;
		Matrix3 coordsystem;
// TODO: Remove this!
Matrix3 cs_abs;
		std::string parent;
		Nameset children;
	};
	typedef std::map< std::string, Bone > Bones;

	Bones bones;
	Nameset rootbones;

	Actions actions;

	inline static void readBonecontrolchannelVersion100(Action::Channel& result, float& min_time, float& max_time, std::istream& strm);

};

inline void Skeleton::readVersion100(std::istream& strm)
{
	bones.clear();
	actions.clear();

	uint32_t bones_size = deserializeUInt32(strm);
	while (bones.size() < bones_size) {

		std::string bone_name = deserializeString(strm, 4);
		if (bones.find(bone_name) != bones.end()) {
			throw Exception("Bone \"" + bone_name + "\" defined twice!");
		}

		Bone new_bone;
		new_bone.pos_abs.deserialize(strm);
		new_bone.coordsystem.deserialize(strm);
		new_bone.parent = deserializeString(strm, 4);

		bones[bone_name] = new_bone;
	}

	uint32_t actions_size = deserializeUInt32(strm);
	while (actions.size() < actions_size) {

		std::string action_name = deserializeString(strm, 4);
		if (actions.find(action_name) != actions.end()) {
			throw Exception("Action \"" + action_name + "\" defined twice!");
		}

		Action new_action;
		new_action.begin = 999999;
		new_action.end = -999999;

		uint32_t bones_size = deserializeUInt32(strm);
		while (new_action.bones.size() < bones_size) {

			std::string bone_name = deserializeString(strm, 4);
			if (new_action.bones.find(bone_name) != new_action.bones.end()) {
				throw Exception("Controlling of bone \"" + bone_name + "\" defined twice in action \"" + action_name + "\"!");
			}

			Action::Bonecontrol new_bonecontrol;
			readBonecontrolchannelVersion100(new_bonecontrol.loc_x, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.loc_y, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.loc_z, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.quat_x, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.quat_y, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.quat_z, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.quat_w, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.scale_x, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.scale_y, new_action.begin, new_action.end, strm);
			readBonecontrolchannelVersion100(new_bonecontrol.scale_z, new_action.begin, new_action.end, strm);

			new_action.bones[bone_name] = new_bonecontrol;

		}

		actions[action_name] = new_action;
	}

}

inline Action const* Skeleton::getAction(std::string const& act_name) const
{
	Actions::const_iterator actions_find = actions.find(act_name);
	if (actions_find == actions.end()) {
		throw Exception("Unable to find action \"" + act_name + "\" from skeleton!");
	}
	return &actions_find->second;
}

inline void Skeleton::readBonecontrolchannelVersion100(Action::Channel& result, float& min_time, float& max_time, std::istream& strm)
{
	result.clear();
	uint32_t points = deserializeUInt32(strm);
	while (result.size() < points) {
		float time = deserializeFloat(strm);
		min_time = std::min(min_time, time);
		max_time = std::max(max_time, time);
		if (result.find(time) != result.end()) {
			throw Exception("Point " + floatToStr(time) + " in channel defined twice!");
		}
		Action::Channelpoint new_point;
		new_point.value = deserializeFloat(strm);
		new_point.weight = deserializeFloat(strm);
		result[time] = new_point;
	}
}

}

#endif
