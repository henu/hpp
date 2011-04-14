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

	// Forms help string of all allowed arguments.
	inline std::string getHelp(void) const;

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

inline std::string Arguments::getHelp(void) const
{
	std::string result;
	bool first_arg = true;
	for (AllowedArgs::const_iterator aas_it = aas.begin();
	     aas_it != aas.end();
	     aas_it ++) {
	     	if (!first_arg) {
	     		result += '\n';
	     	}
	     	// Print name, aliases and required parameters of argument.
		std::string const& arg = aas_it->first;
		AllowedArg const& aa = aas_it->second;
		result += arg;
		for (Aliases::const_iterator aliases_it = aliases.begin();
		     aliases_it != aliases.end();
		     aliases_it ++) {
		     	if (aliases_it->second == arg) {
		     		result += ", " + aliases_it->first;
		     	}
		}
		result += " " + aa.params + '\n';
		// Print description of argument
// TODO: Make nice format in future!
		result += "    " + aa.description + '\n';
		first_arg = false;
	}
	return result;
}

}

#endif
