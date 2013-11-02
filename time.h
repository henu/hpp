#ifndef HPP_TIME_H
#define HPP_TIME_H

#include "assert.h"
#include "exception.h"
#include "cast.h"

#include <stdint.h>
#include <cmath>
#include <ctime>
#include <cerrno>
#include <ostream>
#ifdef WIN32
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#endif

namespace Hpp
{

class Delay
{

public:

	inline Delay(void) { }
	inline Delay(int64_t secs, uint32_t nsecs) : secs_(secs), nsecs_(nsecs), inf_(false) { }

	inline static Delay days(int64_t days) { return Delay(days*24*60*60, 0); }
	inline static Delay hours(int64_t hours) { return Delay(hours*60*60, 0); }
	inline static Delay mins(int64_t mins) { return Delay(mins*60, 0); }
	inline static Delay secs(int64_t secs) { return Delay(secs, 0); }
	inline static Delay msecs(int64_t msecs) { return Delay(msecs / 1000, (msecs % 1000) * 1000 * 1000); }
	inline static Delay usecs(int64_t usecs) { return Delay(usecs / (1000 * 1000), (usecs % (1000 * 1000)) * 1000); }
	inline static Delay nsecs(int64_t nsecs) { return Delay(nsecs / (1000 * 1000 * 1000), nsecs % (1000 * 1000 * 1000)); }
	inline static Delay inf(void) { return Delay(true); }
	inline static Delay fromSecondsAsDouble(double secs);

	inline int64_t getSeconds(void) const { HppAssert(!inf_, "Unable to return seconds of infinite delay!"); return secs_; }
	inline uint32_t getNanoseconds(void) const { HppAssert(!inf_, "Unable to return nanoseconds of infinite delay!"); HppAssert(nsecs_ < MLRD, "Too many nanoseconds!"); return nsecs_; }
	inline bool isInfinite(void) const { return inf_; }

	inline double getSecondsAsDouble(void) const { return getSeconds() + getNanoseconds() / (double)MLRD; }

	// Sleep
	inline void sleep(void) const;

	// Operators
	inline Delay operator+(Delay const& d) const;
	inline Delay operator+=(Delay const& d);
	inline Delay operator-(Delay const& d) const;
	inline Delay operator-=(Delay const& d);
	inline Delay operator*(double d) const;
	inline Delay operator/(int64_t d) const;
	inline double operator/(Delay const& d) const;
	inline Delay operator-(void) const;

	// Comparison operators
	inline bool operator==(Delay const& d) const;
	inline bool operator!=(Delay const& d) const;
	inline bool operator<(Delay const& d) const;
	inline bool operator>(Delay const& d) const;
	inline bool operator<=(Delay const& d) const;
	inline bool operator>=(Delay const& d) const;

private:

	// Private constructor for creating infinite delay
	inline Delay(bool inf) : inf_(inf) { }

	static uint32_t const MLRD = 1000*1000*1000;

	int64_t secs_;
	uint32_t nsecs_;
	bool inf_;

};

class Time
{

public:

	inline Time(void) { }
	inline Time(uint64_t secs, uint32_t nsecs) : secs(secs), nsecs(nsecs) { }

	inline uint64_t getSeconds(void) const { return secs; }
	inline uint32_t getNanoseconds(void) const { HppAssert(nsecs < MLRD, "Too many nanoseconds!"); return nsecs; }

	inline std::string toString(std::string const& format = "%F %T") const;

	// Comparison operators
	inline bool operator==(Time const& t) const;
	inline bool operator!=(Time const& t) const;
	inline bool operator<(Time const& t) const;
	inline bool operator>(Time const& t) const;
	inline bool operator<=(Time const& t) const;
	inline bool operator>=(Time const& t) const;

	// Other operators
	inline Time operator+=(Delay const& d);
	inline Time operator-=(Delay const& d);

private:

	static uint32_t const MLRD = 1000*1000*1000;

	uint64_t secs;
	uint32_t nsecs;

	static inline std::string padWith(size_t num, size_t desired_len, char padder);

};

inline Time now(void);

inline Time operator+(Time const& t, Delay const& d);
inline Time operator-(Time const& t, Delay const& d);
inline std::ostream& operator<<(std::ostream& strm, Time const& t);

inline Delay operator-(Time const& t1, Time const& t2);
inline Delay operator*(double d, Delay const& dl) { return dl * d; }
inline std::ostream& operator<<(std::ostream& strm, Delay const& d);

inline Time now(void)
{
	uint32_t const MLRD = 1000*1000*1000;
	(void)MLRD;
	#ifndef WIN32
	timespec time_now;
/*
	clockid_t clock_id;
	if (clock_getcpuclockid(0, &clock_id) != 0) {
		throw Exception("Unable to get time now, because clock ID is not known!");
	}
*/
	if (clock_gettime(CLOCK_REALTIME, &time_now) != 0) {
		throw Exception("Unable to get time now, because time retrival has failed!");
	}
	HppAssert(time_now.tv_sec != 0, "clock_gettime() says the unix time is now zero!");
	HppAssert(time_now.tv_nsec < MLRD, "clock_gettime() gives too many nanoseconds!");
	return Time(time_now.tv_sec, time_now.tv_nsec);
	#else
	FILETIME time_now;
	GetSystemTimeAsFileTime(&time_now);
	uint64_t time_now_64 = time_now.dwLowDateTime + (uint64_t(time_now.dwHighDateTime) << 32);
	// Convert from microsoft bulls*it to epoch
	time_now_64 = time_now_64 - 11644473600LL;
	uint64_t secs = time_now_64 / uint64_t(10000000);
	uint32_t nsecs = (time_now_64 % 10000000) * 100;
	return Time(secs, nsecs);
	#endif
}

inline Delay Delay::fromSecondsAsDouble(double secs)
{
	int64_t secs2 = floor(secs);
	int64_t nsecs = round((secs - secs2) * MLRD);
	if (nsecs >= MLRD) {
		nsecs -= MLRD;
		secs2 += 1;
	}
	if (nsecs < 0) {
		nsecs += MLRD;
		secs2 -= 1;
	}
	return Delay(secs2, nsecs);
}

inline void Delay::sleep(void) const
{
	if (inf_) {
		throw Exception("You really don\'t want to sleep eternally, do you?");
	}

	// Negative sleep means do not sleep at all
	HppAssert(nsecs_ < MLRD, "Too many nanoseconds!");
	if (secs_ < 0) {
		return;
	}

	#ifndef WIN32
	timespec sleeptime;
	sleeptime.tv_sec = getSeconds();
	sleeptime.tv_nsec = getNanoseconds();
	if (nanosleep(&sleeptime, NULL) != 0) {
/*
	uint64_t msecs = delay.getSeconds() * 1000000 + delay.getNanoseconds() / 1000;
	if (usleep(msecs)) {
*/		if (errno == EINTR) {
			return;
		}
		throw Exception("Sleeping has failed!");
	}
	#else
	uint64_t msecs = getSeconds() * 1000 + getNanoseconds() / 1000000;
	Sleep(msecs);
	#endif
}

inline Delay Delay::operator+(Delay const& d) const
{
	int64_t d_secs = secs_ + d.secs_;
	uint32_t d_nsecs = nsecs_ + d.nsecs_;
	if (d_nsecs >= MLRD) {
		d_nsecs -= MLRD;
		d_secs ++;
	}
	return Delay(d_secs, d_nsecs);
}

inline Delay Delay::operator+=(Delay const& d)
{
	*this = *this + d;
	return *this;
}

inline Delay Delay::operator-(Delay const& d) const
{
	int64_t d_secs = secs_ - d.secs_;
	int32_t d_nsecs = (int32_t)nsecs_ - (int32_t)d.nsecs_;
	HppAssert(d_nsecs > -(int32_t)MLRD, "Fail!");
	if (d_nsecs < 0) {
		d_nsecs += MLRD;
		d_secs --;
	}
	return Delay(d_secs, d_nsecs);
}

inline Delay Delay::operator-=(Delay const& d)
{
	*this = *this - d;
	return *this;
}

inline Delay Delay::operator*(double d) const
{
	if (inf_) {
		return *this;
	}
	if (secs_ >= 0) {
		double secs_new_d = secs_ * d;
		double nsecs_new_d = nsecs_ * d;
		int64_t secs_new = secs_new_d;
		secs_new_d -= secs_new;
		HppAssert(secs_new_d >= 0.0, "Calculation error!");
		nsecs_new_d += secs_new_d * MLRD;
		uint64_t secs_plus = nsecs_new_d / MLRD;
		nsecs_new_d -= secs_plus * MLRD;
		HppAssert(nsecs_new_d < MLRD, "Calculation error!");
		HppAssert(nsecs_new_d >= 0, "Calculation error!");
		return Delay(secs_new, nsecs_new_d);
	}
HppAssert(false, "Not implemented yet!");
// TODO: Code this!
return Delay();
}

inline Delay Delay::operator/(int64_t d) const
{
	if (inf_) {
		return *this;
	}
// Ugly hack before the correct one works
double result_dbl = getSecondsAsDouble();
result_dbl /= (double)d;
uint64_t result_dbl_int = (uint64_t)result_dbl;
return Delay::secs(result_dbl_int) + Delay::nsecs(MLRD * (result_dbl - (double)result_dbl_int));
// TODO: This is broken! Fix it!
	HppAssert(d != 0, "Divizion by zero!");
	int64_t d_secs;
	uint32_t d_nsecs;
	if (secs >= 0) {
		d_secs = secs_ / d;
		d_nsecs = nsecs_ / d;
	} else {
		if (nsecs > 0) {
			d_secs = (secs_+ 1) / d;
			d_nsecs = MLRD - 1 - ((MLRD - nsecs_) / d);
		} else {
			d_secs = secs_ / d;
			d_nsecs = 0;
		}
	}
	return Delay(d_secs, d_nsecs);
}

inline double Delay::operator/(Delay const& d) const
{
// TODO: Make this to be more accurate!
	double this_f = secs_ + nsecs_ / (double)(MLRD);
	double d_f = d.secs_ + d.nsecs_ / (double)(MLRD);
	HppAssert(d_f != 0, "Division by zero!");
	return this_f / d_f;
}

inline Delay Delay::operator-(void) const
{
	if (inf_) {
		return Delay::inf();
	}
	int64_t result_secs = -secs_;
	uint32_t result_nsecs;

	if (nsecs_ > 0) {
		result_nsecs = MLRD - nsecs_;
		-- result_secs;
	} else {
		result_nsecs = 0;
	}

	return Delay(result_secs, result_nsecs);
}

inline bool Delay::operator==(Delay const& d) const
{
	if (inf_) {
		if (d.inf_) return true;
		return false;
	} else if (d.inf_) return false;
	return secs_ == d.secs_ && nsecs_ == d.nsecs_;
}

inline bool Delay::operator!=(Delay const& d) const
{
	if (inf_) {
		if (d.inf_) return false;
		return true;
	} else if (d.inf_) return true;
	return secs_ != d.secs_ && nsecs_ != d.nsecs_;
}

inline bool Delay::operator<(Delay const& d) const
{
	if (inf_) {
		return false;
	} else if (d.inf_) return true;
	if (secs_ < d.secs_) return true;
	if (secs_ > d.secs_) return false;
	return nsecs_ < d.nsecs_;
}

inline bool Delay::operator>(Delay const& d) const
{
	return d < *this;
}

inline bool Delay::operator<=(Delay const& d) const
{
	if (inf_) {
		if (d.inf_) return true;
		return false;
	} else if (d.inf_) return true;
	if (secs_ == d.secs_ && nsecs_ == d.nsecs_) return true;
	if (secs_ < d.secs_) return true;
	if (secs_ > d.secs_) return false;
	return nsecs_ <= d.nsecs_;
}

inline bool Delay::operator>=(Delay const& d) const
{
	return d <= *this;
}

inline std::string Time::toString(std::string const& format) const
{
	#ifndef WIN32
	uint64_t datebuf_size = format.size() * 2 + 16;
	char* datebuf = new char[datebuf_size];
	tm dateobj;
	time_t time_now = secs;
	strftime(datebuf, datebuf_size, format.c_str(), gmtime_r(&time_now, &dateobj));
	std::string result(datebuf);
	delete[] datebuf;
	return result;
	#else
	uint64_t time_64 = (secs * 10000000) + (uint64_t(nsecs) / 100);
	// Convert from epoch to microsoft bulls*it
	time_64 += 11644473600LL;
	FILETIME time_f;
	time_f.dwLowDateTime = time_64 % 0xffffffff;
	time_f.dwHighDateTime = time_64 >> 32;
	SYSTEMTIME time_s;
	if (FileTimeToSystemTime(&time_f, &time_s) == 0) {
		throw Exception("Unable to convert Time to string, because file time could not be converted to systemtime!");
	}

	tm t;
	t.tm_sec = time_s.wSecond;
	t.tm_min = time_s.wMinute;
	t.tm_hour = time_s.wHour;
	t.tm_mday = time_s.wDay;
	t.tm_mon = time_s.wMonth;
	t.tm_year = time_s.wYear;
	t.tm_wday = time_s.wDayOfWeek;
// TODO: Calculate days since january first!
	t.tm_yday = 0;
// TODO: Is now daylight savings time?
	t.tm_isdst = 0;

// TODO: Support time zone!
	// Because strftime is buggy, the conversion
	// is done manually as best as we can
	std::string result = format;
	size_t i = 0;
	while (i < result.size()) {
		char c = result[i];
		if (c == '%') {
			++ i;
			if (i == result.size()) {
				throw Exception("Invalid date format \"" + format + "\"!");
			}
			char c2 = result[i];
			std::string replace_with = "";
			bool rewind = false;
			if (c2 == 'a') {
			} else if (c2 == 'A') {
			} else if (c2 == 'b') {
			} else if (c2 == 'B') {
			} else if (c2 == 'c') {
				replace_with = "%a %b T %Y";
				rewind = true;
			} else if (c2 == 'C') {
				replace_with = padWith(time_s.wYear / 100, 2, '0');
			} else if (c2 == 'd') {
				replace_with = padWith(time_s.wDay, 2, '0');
			} else if (c2 == 'D') {
				replace_with = "%m/%d/%y";
				rewind = true;
			} else if (c2 == 'e') {
				replace_with = padWith(time_s.wDay, 2, ' ');
			} else if (c2 == 'F') {
				replace_with = "%Y-%m-%d";
				rewind = true;
			} else if (c2 == 'g') {
			} else if (c2 == 'h') {
				replace_with = "%b";
				rewind = true;
			} else if (c2 == 'H') {
				replace_with = padWith(time_s.wHour, 2, '0');
			} else if (c2 == 'I') {
				replace_with = padWith(((time_s.wHour + 11) % 12) + 1, 2, '0');;
			} else if (c2 == 'j') {
			} else if (c2 == 'm') {
				replace_with = padWith(time_s.wMonth, 2, '0');
			} else if (c2 == 'M') {
				replace_with = padWith(time_s.wMinute, 2, '0');
			} else if (c2 == 'n') {
				replace_with = "\n";
			} else if (c2 == 'p') {
				if (time_s.wHour < 12) replace_with = "AM";
				else replace_with = "PM";
			} else if (c2 == 'r') {
				replace_with = "%I:%M:%S %p";
				rewind = true;
			} else if (c2 == 'R') {
				replace_with = "%H:%M";
				rewind = true;
			} else if (c2 == 'S') {
				replace_with = padWith(time_s.wSecond, 2, '0');
			} else if (c2 == 't') {
				replace_with = "\t";
			} else if (c2 == 'T') {
				replace_with = "%H:%M:%S";
				rewind = true;
			} else if (c2 == 'u') {
				replace_with = sizeToStr(((time_s.wDayOfWeek + 6) % 7) + 1);
			} else if (c2 == 'U') {
			} else if (c2 == 'V') {
			} else if (c2 == 'w') {
				replace_with = sizeToStr(time_s.wDayOfWeek);
			} else if (c2 == 'W') {
			} else if (c2 == 'x') {
				replace_with = "%m/%d/%y";
				rewind = true;
			} else if (c2 == 'X') {
				replace_with = "%T";
				rewind = true;
			} else if (c2 == 'y') {
				replace_with = padWith(time_s.wYear % 100, 2, '0');
			} else if (c2 == 'Y') {
				replace_with = sizeToStr(time_s.wYear);
			} else if (c2 == 'z') {
			} else if (c2 == 'Z') {
			} else if (c2 == '%') {
				replace_with = "%";
			} else {
				throw Exception("Invalid date specifier \"%" + std::string(1, c2) + "\"!");
			}

			// If no replacement was found, try strftime
			if (replace_with.empty()) {
				char buf[12];
				if (strftime(buf, 12, ("%" + std::string(1, c2)).c_str(), &t) > 0) {
					replace_with = buf;
				}
			}

			result = result.substr(0, i - 1) + replace_with + result.substr(i + 1);
			if (rewind) -- i;
			else i += replace_with.size() - 1;

		} else {
			++ i;
		}
	}
	return result;
	#endif

}

inline bool Time::operator==(Time const& t) const
{
	return secs == t.secs && nsecs == t.nsecs;
}

inline bool Time::operator!=(Time const& t) const
{
	return secs != t.secs && nsecs != t.nsecs;
}

inline bool Time::operator<(Time const& t) const
{
	if (secs < t.secs) return true;
	if (secs > t.secs) return false;
	if (nsecs < t.nsecs) return true;
	return false;
}

inline bool Time::operator>(Time const& t) const
{
	if (secs > t.secs) return true;
	if (secs < t.secs) return false;
	if (nsecs > t.nsecs) return true;
	return false;
}

inline bool Time::operator<=(Time const& t) const
{
	if (secs == t.secs && nsecs == t.nsecs) return true;
	if (secs < t.secs) return true;
	if (secs > t.secs) return false;
	if (nsecs < t.nsecs) return true;
	return false;
}

inline bool Time::operator>=(Time const& t) const
{
	if (secs == t.secs && nsecs == t.nsecs) return true;
	if (secs > t.secs) return true;
	if (secs < t.secs) return false;
	if (nsecs > t.nsecs) return true;
	return false;
}

inline Time Time::operator+=(Delay const& d)
{
// TODO: Optimize?
	return *this = *this + d;
}

inline Time Time::operator-=(Delay const& d)
{
// TODO: Optimize?
	return *this = *this - d;
}

inline std::string Time::padWith(size_t num, size_t desired_len, char padder)
{
	std::string result = sizeToStr(num);
	if (result.size() < desired_len) {
		result = std::string(desired_len - result.size(), padder) + result;
	}
	return result;
}

inline Time operator+(Time const& t, Delay const& d)
{
	HppAssert(!d.isInfinite(), "Unable to add delay to time, because delay is infinite and inifnite times are not yet supported!");
	HppAssert(t.getNanoseconds() < 1000000000, "Too many nanoseconds in Time object!");
	HppAssert(d.getNanoseconds() < 1000000000, "Too many nanoseconds in Delay object!");
	uint32_t nsecs = t.getNanoseconds() + d.getNanoseconds();
	uint64_t secs;
	if (nsecs >= 1000000000) {
		secs = 1;
		nsecs -= 1000000000;
	} else {
		secs = 0;
	}
// TODO: In future, support also dates before 1970!
	if ((int64_t)secs + (int64_t)t.getSeconds() < -d.getSeconds()) {
		throw Exception("Trying to form date before 1970-01-01 00:00! It is not supported yet!");
	}
	secs += t.getSeconds() + d.getSeconds();
	return Time(secs, nsecs);
}

inline Time operator-(Time const& t, Delay const& d)
{
	HppAssert(!d.isInfinite(), "Unable to add delay to time, because delay is infinite and inifnite times are not yet supported!");
	HppAssert(t.getNanoseconds() < 1000000000, "Too many nanoseconds in Time object!");
	HppAssert(d.getNanoseconds() < 1000000000, "Too many nanoseconds in Delay object!");
	int32_t nsecs = t.getNanoseconds() - d.getNanoseconds();
	int64_t secs;
	if (nsecs < 0) {
		secs = -1;
		nsecs += 1000000000;
	} else {
		secs = 0;
	}
// TODO: In future, support also dates before 1970!
	if (secs + (int64_t)t.getSeconds() < d.getSeconds()) {
		throw Exception("Trying to form date before 1970-01-01 00:00! It is not supported yet!");
	}
	secs += t.getSeconds() - d.getSeconds();
	return Time(secs, nsecs);
}

inline std::ostream& operator<<(std::ostream& strm, Time const& t)
{
	strm << t.toString();
	return strm;
}

inline Delay operator-(Time const& t1, Time const& t2)
{
	uint32_t const MLRD = 1000*1000*1000;
	int64_t d_secs = (int64_t)t1.getSeconds() - (int64_t)t2.getSeconds();
	HppAssert(t1.getNanoseconds() < MLRD, "Too many nanoseconds in first Time object!");
	HppAssert(t2.getNanoseconds() < MLRD, "Too many nanoseconds in second Time object!");
	int32_t d_nsecs = (int32_t)t1.getNanoseconds() - (int32_t)t2.getNanoseconds();
	HppAssert(d_nsecs > -(int32_t)MLRD, "Fail!");
	if (d_nsecs < 0) {
		d_nsecs += MLRD;
		d_secs --;
	}
	return Delay(d_secs, d_nsecs);
}

inline std::ostream& operator<<(std::ostream& strm, Delay const& d)
{
	if (d.isInfinite()) {
		strm << "*infinite*";
		return strm;
	}
// TODO: Code this!
HppAssert(d.getSeconds() >= 0, "Negative Delay to stream is not supported yet!");
	uint64_t years = d.getSeconds() / (60*60*24*365);
	uint64_t days = (d.getSeconds() / (60*60*24)) % 365;
	uint64_t hours = (d.getSeconds() / (60*60)) % 24;
	uint64_t mins = (d.getSeconds() / 60) % 60;
	uint64_t secs = d.getSeconds() % 60;
	if (years > 0) {
		strm << years << "a " << days << "d " << hours << "h " << mins << "min ";
	} else if (days > 0) {
		strm << days << "d " << hours << "h " << mins << "min ";
	} else if (hours > 0) {
		strm << hours << "h " << mins << "min ";
	} else if (mins > 0) {
		strm << mins << "min ";
	}
	strm << secs;
	char subsecs[9];
	int8_t nonzeros = 0;
	uint64_t nsecs = d.getNanoseconds();
	for (uint8_t subsecs_offset = 0; subsecs_offset < 9; subsecs_offset ++) {
		uint8_t subsec = nsecs % 10;
		subsecs[8 - subsecs_offset] = '0' + subsec;
		nsecs /= 10;
		if (subsec != 0 && nonzeros == 0) {
			nonzeros = 9 - subsecs_offset;
		}
	}
	if (nonzeros > 0) {
		strm << '.' << std::string(subsecs, nonzeros);
	}
	strm << 's';
	return strm;
}

}

#endif
