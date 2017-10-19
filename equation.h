#ifndef HPP_EQUATION_H
#define HPP_EQUATION_H

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Hpp
{

class Equation
{

public:

	typedef std::set< std::string > Paramnames;
	typedef std::map< std::string, double > Params;

	Equation(std::string const& eq);
	~Equation(void);

	double eval(Params const& params);

	Paramnames getParameters(void) const;

	std::string toString(Params const& params = Params());

private:

	// Type for the member of equation
	enum EqNodeType { NUMBER, SUM, MINUS, MULTIPLY, DIVISION, POWER, VARIABLE, ABS, LOG, SIN, COS };
	struct EqNode {
		EqNodeType type;
		double value;
		union {
			struct {
				EqNode* param;
			} oper1;
			struct {
				EqNode* param[2];
			} oper2;
			struct {
				std::string* name;
			} name;
		} data;
	};

	typedef std::vector< EqNode* > CalcOrder;

	// The equation tree
	EqNode* root;

	// Calculation order of tree
	CalcOrder order;

	// Parses equation in string format and returns it equation tree format.
	static EqNode* parseString(std::string eq);

	static size_t findStringFromEquation(std::string const& eq, std::string const& value);

	static void releaseNode(EqNode* node);

	static void makeOrder(CalcOrder& order, EqNode* node);

	static std::string nodeToString(EqNode const* node, Params const& params);

};

}

#endif
