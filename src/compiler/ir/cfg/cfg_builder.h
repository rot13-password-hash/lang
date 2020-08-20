#pragma once

#include "../cfg/cfg.h"
#include "../ast/ast.h"

namespace lang::compiler::cfg_builder
{
	using function_block_map = std::unordered_map<ir::ast::statement::function_definition*, std::shared_ptr<ir::cfg::block>>;

	class cfg_builder
	{
		ir::ast::statement::restricted_block* root;
	public:
		cfg_builder(ir::ast::statement::restricted_block* root) :
			root(root) {}

		function_block_map build();
	};
}
