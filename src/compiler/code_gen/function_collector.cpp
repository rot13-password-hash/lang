#include "function_collector.h"

using namespace lang::compiler;

bool code_gen::function_collector::visit(ir::ast::statement::function_definition* node)
{
	collected.push_back(node);

	return true;
}