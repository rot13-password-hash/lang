#include "ast.h"

namespace seam::compiler::ir::ast
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

	void expression::variable::visit_children(visitor* vst)
	{
		var->visit(vst);
	}

	void expression::variable::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void expression::function_variable::visit(visitor* vst)
	{
		vst->visit(this);
	}
	

	void statement::variable_declaration::visit_children(visitor* vst)
	{
		value->visit(vst);
	}

	void statement::variable_declaration::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::variable_assignment::visit_children(visitor* vst)
	{
		value->visit(vst);
	}

	void statement::variable_assignment::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::expression_statement::visit_children(visitor* vst)
	{
		expr->visit(vst);
	}

	void statement::expression_statement::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
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

	void statement::extern_definition::visit(visitor* vst)
	{
		vst->visit(this);
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
		body->visit(vst);
	}

	void statement::class_type_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void statement::restricted_block::visit_children(visitor* vst)
	{
		for (const auto& stat : body)
		{
			stat->visit(vst);
		}
	}

	void statement::restricted_block::visit(visitor* vst)
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
