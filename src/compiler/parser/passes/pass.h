#pragma once

#include <cstddef>
#include <string>

#include "../../ir/ast.h"

namespace lang::compiler::parser
{
	struct pass
	{
		// returns true if ast modified
		virtual bool invoke(ir::ast::statement::top_level_block* root);

		virtual void invoke_single(ir::ast::statement::top_level_block* root);

		static void invoke_all(ir::ast::statement::top_level_block* root);
	};


}
