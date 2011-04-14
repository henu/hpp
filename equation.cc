#include "equation.h"

#include "exception.h"
#include "cast.h"

#include <cmath>

namespace Hpp
{

Equation::Equation(std::string const& eq)
{
	// Form equation tree
	root = parseString(eq);

	// Find calculation order
	makeOrder(order, root);

}

Equation::~Equation(void)
{
	releaseNode(root);
}

double Equation::eval(Params const& params)
{
	for (CalcOrder::const_iterator order_it = order.begin();
	     order_it != order.end();
	     order_it ++) {
		EqNode* node = *order_it;

		switch (node->type) {
		case SUM:
			node->value = node->data.oper2.param[0]->value + node->data.oper2.param[1]->value;
			break;
		case MULTIPLY:
			node->value = node->data.oper2.param[0]->value * node->data.oper2.param[1]->value;
			break;
		case DIVISION:
			{
				float divider = node->data.oper2.param[1]->value;
				if (divider == 0) {
					throw Hpp::Exception("Division by zero!");
				}
				node->value = node->data.oper2.param[0]->value / divider;
			}
			break;
		case POWER:
			node->value = pow(node->data.oper2.param[0]->value, node->data.oper2.param[1]->value);
			break;
		case VARIABLE:
			{
				std::string const& param_name = *node->data.name.name;
				Params::const_iterator params_find = params.find(param_name);
				if (params_find == params.end()) {
					throw Hpp::Exception("Variable \"" + param_name + "\" not found!");
				}
				node->value = params_find->second;
			}
			break;
		case LOG:
			node->value = log(node->data.oper1.param->value);
			break;
		default:
			HppAssert(false, "Invalid node type!");
		}

	}

	return root->value;
}

Equation::Paramnames Equation::getParameters(void) const
{
	Paramnames result;
	for (CalcOrder::const_iterator order_it = order.begin();
	     order_it != order.end();
	     order_it ++) {
		EqNode const* node = *order_it;
		if (node->type == VARIABLE) {
			result.insert(*node->data.name.name);
		}
	}

	return result;
}

std::string Equation::toString(Params const& params)
{
	return nodeToString(root, params);
}

Equation::EqNode* Equation::parseString(std::string eq)
{

	// Remove brackets and whitespace from the end and beginning
	std::string::iterator eq_begin = eq.begin();
	std::string::reverse_iterator eq_rbegin = eq.rbegin();
	size_t eq_left = eq.size();
	while (eq_left > 0) {

		if (*eq_begin == ' ' || *eq_begin == '\t' || *eq_begin == '\n') {
			eq_begin ++;
			eq_left --;
		} else if (*eq_rbegin == ' ' || *eq_rbegin == '\t' || *eq_rbegin == '\n') {
			eq_rbegin ++;
			eq_left --;
		} else if ((*eq_begin == '(' && *eq_rbegin == ')') ||
		           (*eq_begin == '[' && *eq_rbegin == ']') ||
		           (*eq_begin == '{' && *eq_rbegin == '}')) {
			eq_begin ++;
			eq_rbegin ++;
			eq_left -= 2;
		} else {
			break;
		}
	}
	eq = std::string(eq_begin, eq_begin + eq_left);

	size_t pos;
	size_t pos2;

	// Check for sum
	pos = findStringFromEquation(eq, "+");
	if (pos != std::string::npos) {
		EqNode* param[2] = { NULL, NULL };
		try {
			param[0] = parseString(eq.substr(0, pos));
			param[1] = parseString(eq.substr(pos + 1));
		}
		catch ( ... ) {
			releaseNode(param[0]);
			throw;
		}
		EqNode* result = new EqNode;
		result->type = SUM;
		result->data.oper2.param[0] = param[0];
		result->data.oper2.param[1] = param[1];
		return result;
	}

	// Check for multiply or division
	pos = findStringFromEquation(eq, "*");
	pos2 = findStringFromEquation(eq, "/");
	if (pos != std::string::npos &&
	    (pos2 == std::string::npos || pos < pos2)) {
		EqNode* param[2] = { NULL, NULL };
		try {
			param[0] = parseString(eq.substr(0, pos));
			param[1] = parseString(eq.substr(pos + 1));
		}
		catch ( ... ) {
			releaseNode(param[0]);
			throw;
		}
		EqNode* result = new EqNode;
		result->type = MULTIPLY;
		result->data.oper2.param[0] = param[0];
		result->data.oper2.param[1] = param[1];
		return result;
	} else if (pos2 != std::string::npos) {
		EqNode* param[2] = { NULL, NULL };
		try {
			param[0] = parseString(eq.substr(0, pos2));
			param[1] = parseString(eq.substr(pos2 + 1));
		}
		catch ( ... ) {
			releaseNode(param[0]);
			throw;
		}
		EqNode* result = new EqNode;
		result->type = DIVISION;
		result->data.oper2.param[0] = param[0];
		result->data.oper2.param[1] = param[1];
		return result;
	}

	// Check for power
	pos = findStringFromEquation(eq, "^");
	if (pos != std::string::npos) {
		EqNode* param[2] = { NULL, NULL };
		try {
			param[0] = parseString(eq.substr(0, pos));
			param[1] = parseString(eq.substr(pos + 1));
		}
		catch ( ... ) {
			releaseNode(param[0]);
			throw;
		}
		EqNode* result = new EqNode;
		result->type = POWER;
		result->data.oper2.param[0] = param[0];
		result->data.oper2.param[1] = param[1];
		return result;
	}

	// Check for number
	if (!eq.empty()) {
		bool is_num = true;
		bool dot_found = false;
		for (std::string::const_iterator eq_it = eq.begin();
		     eq_it != eq.end();
		     eq_it ++) {
			char c = *eq_it;
			if (c >= '0' && c <= '9') {
			} else if (c == '.' && !dot_found) {
				dot_found = true;
			} else {
				is_num = false;
				break;
			}
		}
		if (is_num) {
			EqNode* result = new EqNode;
			result->type = NUMBER;
			result->value = Hpp::strToFloat(eq);
			return result;
		}
	}

	// Check for variable name
	if (!eq.empty()) {
		bool is_name = true;
		for (std::string::const_iterator eq_it = eq.begin();
		     eq_it != eq.end();
		     eq_it ++) {
			char c = *eq_it;
			if ((c >= 'a' && c <= 'z') ||
			    (c >= 'A' && c <= 'Z') ||
			    c == '_' ||
			    (eq_it != eq.begin() && c >= '0' && c <= '9')) {
			} else {
				is_name = false;
				break;
			}
		}
		if (is_name) {
			std::string* name;
			name = new std::string(eq);
			EqNode* result = new EqNode;
			result->type = VARIABLE;
			result->data.name.name = name;
			return result;
		}
	}

	// Check for function "log"
	if (eq.size() >= 5 && eq.substr(0, 4) == "log(" && *eq.rbegin() == ')') {
		EqNode* param = parseString(eq.substr(4, eq.size() - 5));
		EqNode* result = new EqNode;
		result->type = LOG;
		result->data.oper1.param = param;
		return result;
	}

	throw Hpp::Exception("Unable to parse \"" + eq + "\"!");
	return NULL;

}

size_t Equation::findStringFromEquation(std::string const& eq, std::string const& value)
{
	std::string::const_iterator it = eq.begin();
	ssize_t value_size = value.size();

	size_t brackets = 0;
	while (eq.end() - it > value_size) {

		if (*it == '(' || *it == '[' || *it == '{') {
			brackets ++;
		} else if (*it == ')' || *it == ']' || *it == '}') {
			if (brackets == 0) {
				throw Hpp::Exception("Missing closing bracket!");
			}
			brackets --;
		}

		if (brackets == 0 && value == std::string(it, it + value_size)) {
			return it - eq.begin();
		}

		it ++;

	}

	return std::string::npos;
}

void Equation::releaseNode(EqNode* node)
{
	if (!node) return;
	switch (node->type) {
	case NUMBER:
		break;
	case SUM:
	case MULTIPLY:
	case DIVISION:
	case POWER:
		releaseNode(node->data.oper2.param[0]);
		releaseNode(node->data.oper2.param[1]);
		break;
	case LOG:
		releaseNode(node->data.oper1.param);
		break;
	case VARIABLE:
		delete node->data.name.name;
		break;
	default:
		HppAssert(false, "Invalid node type!");
	}
	delete node;
}

void Equation::makeOrder(CalcOrder& order, EqNode* node)
{
	switch (node->type) {
	case NUMBER:
		break;
	case SUM:
	case MULTIPLY:
	case DIVISION:
	case POWER:
		makeOrder(order, node->data.oper2.param[0]);
		makeOrder(order, node->data.oper2.param[1]);
		order.push_back(node);
		break;
	case VARIABLE:
		order.push_back(node);
		break;
	case LOG:
		makeOrder(order, node->data.oper1.param);
		order.push_back(node);
		break;
	default:
		HppAssert(false, "Invalid node type!");
	}
}

std::string Equation::nodeToString(EqNode const* node, Params const& params)
{
	switch (node->type) {
	case NUMBER:
		return Hpp::floatToStr(node->value);
	case SUM:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " + " + nodeToString(node->data.oper2.param[1], params) + ")";
	case MULTIPLY:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " * " + nodeToString(node->data.oper2.param[1], params) + ")";
	case DIVISION:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " / " + nodeToString(node->data.oper2.param[1], params) + ")";
	case POWER:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " ^ " + nodeToString(node->data.oper2.param[1], params) + ")";
	case VARIABLE:
		{
			std::string const& var = *node->data.name.name;
			Params::const_iterator params_find = params.find(var);
			if (params_find == params.end()) {
				return var;
			} else {
				return Hpp::floatToStr(params_find->second);
			}
		}
	case LOG:
		return "log(" + nodeToString(node->data.oper1.param, params) + ")";
	default:
		HppAssert(false, "Invalid node type!");
	}
	return "";
}

}
