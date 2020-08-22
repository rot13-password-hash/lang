#include "../ir/ast/ast.h"

namespace lang::compiler::code_gen
{
	struct function_collector : ir::ast::visitor
	{
		std::vector<ir::ast::statement::function_definition*> collected;

		bool visit(ir::ast::statement::function_definition* node);
	};
}