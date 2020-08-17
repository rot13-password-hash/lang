#pragma once

#include "pass.h"

#include <iostream>

namespace lang::compiler::parser
{
	class type_analyzer : public pass
	{
		void invoke_single(ir::ast::statement::restricted_block* root) override;
	};
}