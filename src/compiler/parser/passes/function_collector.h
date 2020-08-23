#pragma once

#include "../../ir/ast/ast.h"

namespace seam::compiler::code_gen
{
	struct symbol_collector : ir::ast::visitor
	{
		std::vector<std::string> symbol_stack;

		std::unordered_map<std::string, ir::ast::statement::function_definition*> collected;

		bool visit(ir::ast::statement::function_definition* node);
		bool visit(ir::ast::statement::class_type_definition* node);
	};
}