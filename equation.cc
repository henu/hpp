#include "equation.h"

#include "exception.h"
#include "cast.h"

#include <cmath>

namespace Hpp
{

Equation::Equation(std::string const& eq)
{
	// Form equation tree
	std::string::const_iterator eq_it = eq.begin();
	root = parseString(eq_it, eq.end());

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
		case MINUS:
			node->value = node->data.oper2.param[0]->value - node->data.oper2.param[1]->value;
			break;
		case MULTIPLY:
			node->value = node->data.oper2.param[0]->value * node->data.oper2.param[1]->value;
			break;
		case DIVISION:
			{
				float divider = node->data.oper2.param[1]->value;
				if (divider == 0) {
					throw Exception("Division by zero!");
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
					throw Exception("Variable \"" + param_name + "\" not found!");
				}
				node->value = params_find->second;
			}
			break;
		case ABS:
			node->value = fabs(node->data.oper1.param->value);
			break;
		case LOG:
			node->value = log(node->data.oper1.param->value);
			break;
		case SIN:
			node->value = sin(node->data.oper1.param->value);
			break;
		case COS:
			node->value = cos(node->data.oper1.param->value);
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

Equation::EqNode* Equation::parseString(std::string::const_iterator& it, std::string::const_iterator const& end)
{
	CalcOrder operands;
	std::vector< char > operators;

	try {
		while (true) {

			// Skip whitespace
			while (it != end && (*it == ' ' || *it == '\t' || *it == '\n')) {
				++ it;
			}
			if (it == end) {
				throw Exception("Empty equation operand!");
			}

			// Start by reading an operand

// TODO: Support special case of unary minus operator
			// Opening bracket
			if (*it == '(') {
				char bracket = *it;
				++ it;

				operands.push_back(parseString(it, end));
				if (it == end) {
					throw Exception("Unclosed bracket!");
				} else if (*it != ')') {
					throw Exception("Unclosed bracket!");
				}
				++ it;
			}
			// Number
			else if ((*it >= '0' && *it <= '9') || *it == '.') {
				std::string num_str(1, *it);
				bool dot_met = (*it == '.');
				++ it;
				while (it != end && ((*it >= '0' && *it <= '9') || (!dot_met && *it == '.'))) {
					num_str += *it;
					++ it;
				}
				EqNode* num = new EqNode;
				num->type = NUMBER;
				num->value = strToFloat(num_str);
				operands.push_back(num);
			}
			// "abs()" function
			else if (end - it >= 4 && std::string(it, it + 4) == "abs(") {
				it += 4;
				EqNode* param = parseString(it, end);
				EqNode* func = new EqNode;
				func->type = ABS;
				func->data.oper1.param = param;
				operands.push_back(func);
				if (it == end || *it != ')') {
					throw Exception("Unclosed bracket!");
				}
				++ it;
			}
			// "log()" function
			else if (end - it >= 4 && std::string(it, it + 4) == "log(") {
				it += 4;
				EqNode* param = parseString(it, end);
				EqNode* func = new EqNode;
				func->type = LOG;
				func->data.oper1.param = param;
				operands.push_back(func);
				if (it == end || *it != ')') {
					throw Exception("Unclosed bracket!");
				}
				++ it;
			}
			// "sin()" function
			else if (end - it >= 4 && std::string(it, it + 4) == "sin(") {
				it += 4;
				EqNode* param = parseString(it, end);
				EqNode* func = new EqNode;
				func->type = SIN;
				func->data.oper1.param = param;
				operands.push_back(func);
				if (it == end || *it != ')') {
					throw Exception("Unclosed bracket!");
				}
				++ it;
			}
			// "cos()" function
			else if (end - it >= 4 && std::string(it, it + 4) == "cos(") {
				it += 4;
				EqNode* param = parseString(it, end);
				EqNode* func = new EqNode;
				func->type = COS;
				func->data.oper1.param = param;
				operands.push_back(func);
				if (it == end || *it != ')') {
					throw Exception("Unclosed bracket!");
				}
				++ it;
			}
			// Variable
			else if ((*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z') || *it == '_') {
				std::string var_name(1, *it);
				++ it;
				while (it != end && ((*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z') || *it == '_') || (*it >= '0' && *it <= '9')) {
					var_name += *it;
					++ it;
				}
				EqNode* var = new EqNode;
				var->type = VARIABLE;
				var->data.name.name = new std::string(var_name);
				operands.push_back(var);
			} else {
				throw Exception("Unsupported operand!");
			}

			// Skip whitespace
			while (it != end && (*it == ' ' || *it == '\t' || *it == '\n')) {
				++ it;
			}

			// Read operator, or skip if there is no operators left
			if (it == end || *it == ')') {
				break;
			}
			char oper = *it;
			if (oper != '+' && oper != '-' && oper != '*' && oper != '/' && oper != '^') {
				throw Exception("Invalid operator!");
			}
			operators.push_back(oper);
			++ it;
		}
	}
	catch (...) {
		for (unsigned i = 0; i < operands.size(); ++ i) {
			releaseNode(operands[i]);
		}
		throw;
	}

	// Combine operands and operators. Loop and find
	// strongest operators and combine them first.
	while (operands.size() > 1) {
		HppAssert(operands.size() - 1 = operators.size(), "Operators/operands mismatch!");

		// Find strongest operator
		char strongest_operator = operators[0];
		unsigned strongest_operator_i = 0;
		for (unsigned operator_i = 1; operator_i < operators.size(); ++ operator_i) {
			char oper = operators[operator_i];
			if (oper == '^') {
				if (strongest_operator != '^') {
					strongest_operator = oper;
					strongest_operator_i = operator_i;
				}
			} else if (oper == '*' || oper == '/') {
				if (strongest_operator == '+' || strongest_operator == '-') {
					strongest_operator = oper;
					strongest_operator_i = operator_i;
				}
			}
		}

		// Convert to node
		EqNode* oper = new EqNode;
		if (strongest_operator == '+') {
			oper->type = SUM;
		} else if (strongest_operator == '-') {
			oper->type = MINUS;
		} else if (strongest_operator == '*') {
			oper->type = MULTIPLY;
		} else if (strongest_operator == '/') {
			oper->type = DIVISION;
		} else if (strongest_operator == '^') {
			oper->type = POWER;
		}
		oper->data.oper2.param[0] = operands[strongest_operator_i];
		oper->data.oper2.param[1] = operands[strongest_operator_i + 1];

		// Replace operand in list and remove used operator
		operands[strongest_operator_i] = oper;
		operands.erase(operands.begin() + strongest_operator_i + 1);
		operators.erase(operators.begin() + strongest_operator_i);
	}

	return operands[0];
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
				throw Exception("Missing closing bracket!");
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
	case MINUS:
	case MULTIPLY:
	case DIVISION:
	case POWER:
		releaseNode(node->data.oper2.param[0]);
		releaseNode(node->data.oper2.param[1]);
		break;
	case ABS:
	case LOG:
	case SIN:
	case COS:
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
	case MINUS:
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
	case ABS:
	case LOG:
	case SIN:
	case COS:
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
		return floatToStr(node->value);
	case SUM:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " + " + nodeToString(node->data.oper2.param[1], params) + ")";
	case MINUS:
		return "(" + nodeToString(node->data.oper2.param[0], params) + " - " + nodeToString(node->data.oper2.param[1], params) + ")";
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
				return floatToStr(params_find->second);
			}
		}
	case ABS:
		return "abs(" + nodeToString(node->data.oper1.param, params) + ")";
	case LOG:
		return "log(" + nodeToString(node->data.oper1.param, params) + ")";
	case SIN:
		return "sin(" + nodeToString(node->data.oper1.param, params) + ")";
	case COS:
		return "cos(" + nodeToString(node->data.oper1.param, params) + ")";
	default:
		HppAssert(false, "Invalid node type!");
	}
	return "";
}

}
