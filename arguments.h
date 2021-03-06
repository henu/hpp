#ifndef HPP_ARGUMENTS_H
#define HPP_ARGUMENTS_H

#include "cast.h"
#include "misc.h"

#include <list>
#include <map>
#include <string>
#include <stdexcept>

namespace Hpp
{

class Arguments
{

public:

	// What to output in help
	static uint8_t const INC_ARG             = 0x01;
	static uint8_t const INC_ALIASES         = 0x02;
	static uint8_t const INC_PARAMS          = 0x04;
	static uint8_t const INC_DESC            = 0x08;
	static uint8_t const INC_ARG_AND_ALIASES = 0x03;
	static uint8_t const INC_ALL_BUT_DESC    = 0x07;
	static uint8_t const INC_ALL             = 0xff;

	inline Arguments(int argc, char const** argv);
	inline Arguments(int argc, char** argv);

	inline void addArgument(std::string const& name,
	                        std::string const& params = "",
	                        std::string const& description = "");
	inline void addAlias(std::string const& alias, std::string const& arg);

	// Extracts one argument. Does conversion from alias to correct
	// argument. Returns empty string if there is no more arguments left.
	// If unknown argument is met, then it is moved to extra arguments.
	inline std::string parse(void);

	// Pops next raw argumenet from queue. If there are
	// no arguments left, then exception is thrown.
	inline std::string popArgument(void);
	inline ssize_t popArgumentAsSSize(void);
	inline size_t argsLeft(void) const;

	// Handling of extra arguments. These should be called only after all
	// other arguments are read ie. after parse() returns empty string!
	inline std::string popExtraargument(void);
	inline ssize_t popExtraargumentAsSSize(void);
	inline size_t extraargsLeft(void) const;

	// Forms help string of all allowed arguments,
	// or just from onle argument, if given.
	inline std::string getHelp(uint8_t inc_what = INC_ALL, std::string const& arg = "", std::string const& prefix = "") const;

private:

	struct AllowedArg
	{
		std::string params;
		std::string description;
	};
	typedef std::map< std::string, AllowedArg > AllowedArgs;

	typedef std::map< std::string, std::string > Aliases;

	typedef std::list< std::string > Args;

	Args args;

	Args eargs;

	AllowedArgs aas;
	Aliases aliases;

};

inline Arguments::Arguments(int argc, char const** argv) :
args(argv + 1, argv + argc)
{
}

inline Arguments::Arguments(int argc, char** argv) :
args(argv + 1, argv + argc)
{
}

inline void Arguments::addArgument(std::string const& name,
                                   std::string const& params,
                                   std::string const& description)
{
	AllowedArg new_aa;
	new_aa.params = params;
	new_aa.description = description;
	aas[name] = new_aa;
}

inline void Arguments::addAlias(std::string const& alias, std::string const& arg)
{
	aliases[alias] = arg;
}

inline std::string Arguments::parse(void)
{
	while (!args.empty()) {

		std::string result = args.front();
		args.pop_front();

		// Check if this is argument
		AllowedArgs::const_iterator aas_find = aas.find(result);
		if (aas_find != aas.end()) {
			return result;
		}

		// Check if this is alias
		Aliases::const_iterator aliases_find = aliases.find(result);
		if (aliases_find != aliases.end()) {
			return aliases_find->second;
		}

		// This is not argument nor alias, so move it to extra arguments
		eargs.push_back(result);
	}

	return "";
}

inline std::string Arguments::popArgument(void)
{
	if (args.empty()) {
		throw Exception("No arguments to pop!");
	}
	std::string result = args.front();
	args.pop_front();
	return result;
}

inline ssize_t Arguments::popArgumentAsSSize(void)
{
	std::string result_str = popArgument();
	return strToSSize(result_str);
}

inline size_t Arguments::argsLeft(void) const
{
	return args.size();
}

inline std::string Arguments::popExtraargument(void)
{
	if (!args.empty()) {
		throw Exception("You should access extra arguments only after normal arguments are read!");
	}
	if (eargs.empty()) {
		throw Exception("No extra arguments to pop!");
	}
	std::string result = eargs.front();
	eargs.pop_front();
	return result;
}

inline ssize_t Arguments::popExtraargumentAsSSize(void)
{
	std::string result_str = popExtraargument();
	return strToSSize(result_str);
}

inline size_t Arguments::extraargsLeft(void) const
{
	if (!args.empty()) {
		throw Exception("You should access extra arguments only after normal arguments are read!");
	}
	return eargs.size();
}

inline std::string Arguments::getHelp(uint8_t inc_what, std::string const& arg, std::string const& prefix) const
{
	std::string result;
	if (arg == "") {
		for (AllowedArgs::const_iterator aas_it = aas.begin();
		     aas_it != aas.end();
		     aas_it ++) {

			std::string const& arg_name = aas_it->first;
			AllowedArg const& aa = aas_it->second;

			// Arguments and parameters line
			std::string argline;
			if (inc_what & INC_ALL_BUT_DESC) {
				argline = getHelp(inc_what & INC_ALL_BUT_DESC, arg_name);
				result += wrapWords(argline, prefix) + '\n';
			}

			// Description
			if (inc_what & INC_DESC) {
				if (!argline.empty()) {
					result += wrapWords(aa.description, prefix + "\t") + '\n';
				} else {
					result += wrapWords(aa.description, prefix) + '\n';
				}
			}
		}
	} else {
		AllowedArgs::const_iterator aas_find = aas.find(arg);
		if (aas_find == aas.end()) {
			Aliases::const_iterator aliases_find = aliases.find(arg);
			if (aliases_find == aliases.end()) {
				throw Exception("Argument not found!");
			}
			aas_find = aas.find(aliases_find->second);
			if (aas_find == aas.end()) {
				throw Exception("Alias found, but it points to non-existing argument!");
			}
		}
		AllowedArg const& aa = aas_find->second;

		if (inc_what & INC_ARG) {
			result += aas_find->first;
		}
		if (inc_what & INC_ALIASES) {
			for (Aliases::const_iterator aliases_it = aliases.begin();
			     aliases_it != aliases.end();
			     ++ aliases_it) {
				if (aliases_it->second == aas_find->first) {
					if (!result.empty()) {
						result += " / ";
					}
					result += aliases_it->first;
				}
			}
		}
		if (inc_what & INC_PARAMS) {
			if (!result.empty()) {
				result += " ";
			}
			result += aa.params;
		}
		if (inc_what & INC_DESC) {
			if (!result.empty()) {
				result += " ";
			}
			result += aa.description;
		}
	}
	return result;
}

}

#endif

