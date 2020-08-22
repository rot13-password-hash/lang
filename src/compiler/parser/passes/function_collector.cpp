#include "function_collector.h"

using namespace lang::compiler;

bool code_gen::function_collector::visit(ir::ast::statement::function_definition* node)
{
	collected.push_back(node);

	return true;
}

bool code_gen::function_collector::visit(ir::ast::statement::class_type_definition* node)
{
	for (const auto& func_def : node->body->body)
	{
		collected.emplace_back(func_def.get());
	}

	return true;
}
