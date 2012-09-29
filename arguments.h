#ifndef HPP_ARGUMENTS_H
#define HPP_ARGUMENTS_H

#include "cast.h"

#include <vector>
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
	inline std::string parse(void);

	// Pops next argumenet from queue without checking if it's alias. If
	// there is no arguments left, then exception is thrown.
	inline std::string popArgument(void);
	inline ssize_t popArgumentAsSSize(void);

	inline size_t argsLeft(void) const;

	// Forms help string of all allowed arguments,
	// or just from onle argument, if given.
	inline std::string getHelp(uint8_t inc_what = INC_ALL, std::string const& arg = "") const;

private:

	struct AllowedArg
	{
		std::string params;
		std::string description;
	};
	typedef std::map< std::string, AllowedArg > AllowedArgs;

	typedef std::map< std::string, std::string > Aliases;

	typedef std::vector< std::string > Args;

	Args args;
	Args::const_iterator args_it;

	AllowedArgs aas;
	Aliases aliases;

};

inline Arguments::Arguments(int argc, char const** argv) :
args(argv + 1, argv + argc),
args_it(args.begin())
{
}

inline Arguments::Arguments(int argc, char** argv) :
args(argv + 1, argv + argc),
args_it(args.begin())
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
	if (args_it == args.end()) {
		return "";
	}
	std::string result = *args_it;
	Aliases::const_iterator aliases_find = aliases.find(result);
	if (aliases_find != aliases.end()) {
		result = aliases_find->second;
	}
	args_it ++;
	return result;
}

inline std::string Arguments::popArgument(void)
{
	if (args_it == args.end()) {
		throw Hpp::Exception("No arguments to pop!");
	}
	return *(args_it ++);
}

inline ssize_t Arguments::popArgumentAsSSize(void)
{
	std::string result_str = popArgument();
	return strToSSize(result_str);
}

inline size_t Arguments::argsLeft(void) const
{
	return args.end() - args_it;
}

inline std::string Arguments::getHelp(uint8_t inc_what, std::string const& arg) const
{
	std::string result;
	if (arg == "") {
		bool first_arg = true;
		for (AllowedArgs::const_iterator aas_it = aas.begin();
		     aas_it != aas.end();
		     aas_it ++) {
		     	if (!first_arg) {
		     		result += '\n';
		     	}
		     	// Print name, aliases and required parameters of argument.
			std::string const& arg2 = aas_it->first;
			AllowedArg const& aa = aas_it->second;
			result += arg2;
			for (Aliases::const_iterator aliases_it = aliases.begin();
			     aliases_it != aliases.end();
			     aliases_it ++) {
			     	if (aliases_it->second == arg2) {
			     		result += ", " + aliases_it->first;
			     	}
			}
			result += " " + aa.params + '\n';
			// Print description of argument
// TODO: Make nice format in future!
// TODO: Use inc_what options!
			result += "    " + aa.description + '\n';
			first_arg = false;
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
