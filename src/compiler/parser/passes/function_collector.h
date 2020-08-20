#pragma once

#include "pass.h"

namespace lang::compiler::parser
{
	class function_collector : public pass
	{
		void invoke_single(ir::ast::statement::restricted_block* root) override;
	};
}