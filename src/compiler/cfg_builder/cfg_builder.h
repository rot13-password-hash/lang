#pragma once

#include "../ir/cfg.h"

namespace lang::compiler::cfg_builder
{
	class cfg_builder
	{
		ir::ast::statement::restricted_block* root;
	public:
		cfg_builder(ir::ast::statement::restricted_block* root) :
			root(root) {}

		std::vector<std::pair<ir::ast::statement::function_definition*, std::shared_ptr<ir::cfg::block>>> build();
	};
}
