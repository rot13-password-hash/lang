#include "ast.h"

namespace lang::compiler::ir::ast
{
	void expression::call::visit_children(visitor* vst)
	{
		func->visit(vst);
		for (auto& arg : arguments)
		{
			arg->visit(vst);
		}
	}

	void expression::call::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void expression::unresolved_variable::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void statement::block::visit_children(visitor* vst)
	{
		for (const auto& stat : body)
		{
			stat->visit(vst);
		}
	}

	void statement::block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::function_definition::visit_children(visitor* vst)
	{
		body_stat->visit(vst);
	}

	void statement::function_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::alias_type_definition::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void statement::class_type_definition::visit_children(visitor* vst)
	{
		for (const auto& field : fields)
		{
			if (field.value)
			{
				field.value->visit(vst);
			}
		}

		for (const auto& stat : body)
		{
			stat->visit(vst);
		}
	}

	void statement::class_type_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::top_level_block::visit_children(visitor* vst)
	{
		for (const auto& stat : body)
		{
			stat->visit(vst);
		}
	}

	void statement::top_level_block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::ret::visit_children(visitor* vst)
	{
		if (value)
		{
			value->visit(vst);
		}
	}

	void statement::ret::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}
}
