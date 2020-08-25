#include "variable_resolver.h"
#include "../../utils/exception.h"

using namespace seam::compiler;

bool parser::variable_resolver::visit(ir::ast::expression::variable* node)
{
	auto unresolved_var = dynamic_cast<ir::ast::expression::unresolved_variable*>(node->var.get());
	if (!unresolved_var)
	{
		throw exception(node->range.start, "variable already resolved");
	}

	auto var = std::move(unresolved_var);

	// first check if its a local variable (not implemented yet) 
	// then if its a module function
	// then if its a imported module function

	auto it = symbol_map.find(unresolved_var->name);
	if (it != symbol_map.cend())
	{
		node->var = std::make_unique<ir::ast::expression::function_variable>(unresolved_var->range, it->first, it->second);
		return false;
	}
	throw exception(node->range.start, "could not find variable '" + unresolved_var->name + "', did you forget to declare it?");
}