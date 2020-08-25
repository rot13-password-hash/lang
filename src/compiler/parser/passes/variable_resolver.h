#pragma once
#include "../../ir/ast/ast.h"

namespace seam::compiler::parser
{
	class variable_resolver : public ir::ast::visitor
	{
		const std::unordered_map<std::string, ir::ast::statement::function_declaration*>& symbol_map;
	public:
		bool visit(ir::ast::expression::variable* node);

		variable_resolver(const std::unordered_map<std::string, ir::ast::statement::function_declaration*>& symbol_map) :
			symbol_map(symbol_map) {}
	};
}
