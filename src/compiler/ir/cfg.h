#pragma once

#include "ast.h"

#include <memory>

namespace lang::compiler::ir::cfg
{
	struct cf_instruction
	{
		virtual ~cf_instruction() {}
	};

	struct if_instruction : cf_instruction
	{
		ast::expression::expression* condition;
	};

	struct jump_instruction : cf_instruction
	{
		
	};

	struct block : std::enable_shared_from_this<block>
	{
		std::vector<std::weak_ptr<block>> predecessors;
		std::vector<std::shared_ptr<block>> successors;

		std::vector<ast::statement::statement*> body;
		std::unique_ptr<cf_instruction> cf_instr;

		void add_successor(std::shared_ptr<block> successor); // succ or successor?
	};
}