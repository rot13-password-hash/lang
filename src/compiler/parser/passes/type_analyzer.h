#pragma once

#include "pass.h"

namespace seam::compiler::parser
{
	class type_analyzer : public pass
	{
		void invoke_single(ir::ast::statement::restricted_block* root) override;
	};
}