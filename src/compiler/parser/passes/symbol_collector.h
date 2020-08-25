#pragma once

#include "../../ir/ast/ast.h"

namespace seam::compiler::parser
{
	struct symbol_collector : ir::ast::visitor
	{
		std::vector<std::string> symbol_stack;

		std::unordered_map<std::string, ir::ast::statement::function_declaration*> collected;

		bool visit(ir::ast::statement::extern_definition* node) override;
		bool visit(ir::ast::statement::function_definition* node) override;
		bool visit(ir::ast::statement::class_type_definition* node) override;
	};
}