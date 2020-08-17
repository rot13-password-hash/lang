#include "pass.h"

#include "type_analyzer.h"

#include <vector>

namespace lang::compiler::parser
{
	bool pass::invoke(ir::ast::statement::restricted_block* root)
	{
		return false;
	}

	void pass::invoke_single(ir::ast::statement::restricted_block* root)
	{

	}

	void pass::invoke_all(ir::ast::statement::restricted_block* root)
	{
		std::vector<std::unique_ptr<pass>> passes{};
		passes.emplace_back(std::make_unique<type_analyzer>());

		for (const auto& pass : passes)
		{
			pass->invoke_single(root);
		}

		bool ast_changed = true;
		while (ast_changed)
		{
			ast_changed = false;
			for (const auto& pass : passes)
			{
				if (pass->invoke(root))
				{
					ast_changed = true;
				}
			}
		}
	}
}