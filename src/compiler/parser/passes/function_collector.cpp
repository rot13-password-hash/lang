#include "function_collector.h"

#include "../../utils/exception.h"
#include "../../utils/error.h"

#include <sstream>

using namespace seam::compiler;

bool code_gen::symbol_collector::visit(ir::ast::statement::function_definition* node)
{
	std::stringstream func_symbol_ss;


	if (node->attributes.find("constructor") != node->attributes.cend())
	{
		if (!symbol_stack.empty())
		{
			// TODO: use error
			throw exception{ node->range.start, "unexpected module constructor inside type" };
		}
		
		func_symbol_ss << "@constructor";
	}
	else
	{
		for (auto& symbol : symbol_stack)
		{
			func_symbol_ss << symbol << '.';
		}
		func_symbol_ss << node->name;
	}

	auto func_symbol = func_symbol_ss.str();

	if (collected.find(func_symbol) != collected.cend())
	{
		// TODO: use error
		std::stringstream error_message;
		error_message << "attempt to redefine symbol '" << func_symbol << '\'';
		throw exception{ node->range.start, error_message.str() };
	}

	collected[func_symbol] = node;
	return true;
}

bool code_gen::symbol_collector::visit(ir::ast::statement::class_type_definition* node)
{
	symbol_stack.push_back(node->name);
	node->visit_children(this);
	symbol_stack.pop_back();
	return false;
}