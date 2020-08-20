#include "function_collector.h"

#include <iostream>

class collector : public ir::visitor
{

};

namespace lang::compiler::parser
{
	void function_collector::invoke_single(ir::ast::statement::restricted_block* root)
	{
		std::cout << "lol" << std::endl;
	}
}