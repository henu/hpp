#ifndef HPP_USERINFO_H
#define HPP_USERINFO_H

#include "cast.h"
#include "exception.h"

#include <string>
#ifndef WIN32
#include <grp.h>
#include <pwd.h>
#endif

namespace Hpp
{

class Userinfo
{

public:

	inline Userinfo(void);

	inline ssize_t getUserId(void) const { return user_id; }
	inline ssize_t getGroupId(void) const { return group_id; }
	inline std::string getUser(void) const { return user; }
	inline std::string getGroup(void) const { return group; }

	inline static Userinfo getMyInfo(void);

private:

	ssize_t user_id;
	ssize_t group_id;
	std::string user;
	std::string group;

	#ifndef WIN32
	inline static Userinfo getFromUserId(uid_t user_id);
	#endif

};

inline Userinfo::Userinfo(void) :
user_id(-1),
group_id(-1)
{
}

inline Userinfo Userinfo::getMyInfo(void)
{
	#ifndef WIN32
	uid_t my_uid = geteuid();
	return getFromUserId(my_uid);
	#else
	Userinfo dummy;
	return rummy;
	#endif
}

#ifndef WIN32
inline Userinfo Userinfo::getFromUserId(uid_t user_id)
{
	// Read password struct
	struct passwd* pw = getpwuid(user_id);
	if (!pw) {
		throw Hpp::Exception("User #" + ssizeToStr(user_id) + " does not exist!");
	}

	Userinfo result;

	// User ID and name
	result.user_id = user_id;
	result.user = pw->pw_name;

	// Read group struct
	gid_t group_id = pw->pw_gid;
	struct group* gr = getgrgid(group_id);
	if (!gr) {
		throw Hpp::Exception("Group #" + ssizeToStr(group_id) + " does not exist!");
	}

	// Group ID and name
	result.group_id = group_id;
	result.group = gr->gr_name;

	return result;
}
#endif

}

#endif

