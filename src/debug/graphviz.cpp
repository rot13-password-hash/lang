#include "graphviz.h"

using namespace lang::compiler;

void graphvizitor::write_node(void* id, const std::string& label)
{
	out << 'x' << id << "[label=\"" << label << "\"]\n";
	if (parent_id)
	{
		out << 'x' << parent_id << "->x" << id << '\n';
	}
}

bool graphvizitor::visit(ir::ast::expression::literal<std::string>* literal_string_expr)
{
	write_node(literal_string_expr, "string\\n\\\"" + literal_string_expr->val + "\\\"");
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<ir::ast::number>* literal_number_expr)
{
	write_node(literal_number_expr, "number\\n" + literal_number_expr->val.value);
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::int8_t>* literal_i8_expr)
{
	write_node(literal_i8_expr, std::to_string(literal_i8_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::int16_t>* literal_i16_expr)
{
	write_node(literal_i16_expr, std::to_string(literal_i16_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::int32_t>* literal_i32_expr)
{
	write_node(literal_i32_expr, std::to_string(literal_i32_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::int64_t>* literal_i64_expr)
{
	write_node(literal_i64_expr, std::to_string(literal_i64_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::uint8_t>* literal_u8_expr)
{
	write_node(literal_u8_expr, std::to_string(literal_u8_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::uint16_t>* literal_u16_expr)
{
	write_node(literal_u16_expr, std::to_string(literal_u16_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::uint32_t>* literal_u32_expr)
{
	write_node(literal_u32_expr, std::to_string(literal_u32_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<std::uint64_t>* literal_u64_expr)
{
	write_node(literal_u64_expr, std::to_string(literal_u64_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<float>* literal_f32_expr)
{
	write_node(literal_f32_expr, std::to_string(literal_f32_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<double>* literal_f64_expr)
{
	write_node(literal_f64_expr, std::to_string(literal_f64_expr->val));
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<bool>* literal_bool_expr)
{
	write_node(literal_bool_expr, literal_bool_expr->val ? "true" : "false");
	return false;
}

bool graphvizitor::visit(ir::ast::expression::call* call_expr)
{
	write_node(call_expr, "call");
	auto o_parent_id = parent_id;
	parent_id = call_expr;

	call_expr->visit_children(this);

	parent_id = o_parent_id;
	return false;
}

bool graphvizitor::visit(ir::ast::expression::unresolved_variable* unresolved_var_expr)
{
	write_node(unresolved_var_expr, "unresolved variable\\n" + unresolved_var_expr->name);
	return false;
}

bool graphvizitor::visit(lang::compiler::ir::ast::statement::function_definition* func_def_stat)
{
	std::stringstream label_ss;
	label_ss << "fn " << func_def_stat->name << '(';
	for (std::size_t i = 0; i < func_def_stat->arguments.size(); ++i)
	{
		label_ss << func_def_stat->arguments[i].name << ": " << func_def_stat->arguments[i].type_.name;
		if (func_def_stat->arguments[i].type_.is_optional)
		{
			label_ss << '?';
		}

		if (i + 1 == func_def_stat->arguments.size())
		{
			break;
		}

		label_ss << ", ";
	}
	label_ss << ") -> " << func_def_stat->return_type.name;
	if (func_def_stat->return_type.is_optional)
	{
		label_ss << '?';
	}

	for (auto attribute : func_def_stat->attributes)
	{
		label_ss << " @" << attribute;
	}

	write_node(func_def_stat, label_ss.str());
	auto o_parent_id = parent_id;
	parent_id = func_def_stat;

	func_def_stat->visit_children(this);

	parent_id = o_parent_id;
	return false;
}

bool graphvizitor::visit(ir::ast::statement::alias_type_definition* alias_type_def)
{
	std::stringstream label_ss;
	label_ss << "type " << alias_type_def->alias_name << " = " << alias_type_def->target_type.name;
	write_node(alias_type_def, label_ss.str());
	return false;
}

bool graphvizitor::visit(ir::ast::statement::class_type_definition* class_type_def)
{
	throw std::runtime_error("class type definition not implemented");
	return false;
}

bool graphvizitor::visit(ir::ast::statement::block* block_stat)
{
	write_node(block_stat, "block");
	auto o_parent_id = parent_id;
	parent_id = block_stat;

	block_stat->visit_children(this);

	parent_id = o_parent_id;
	return false;
}

bool graphvizitor::visit(ir::ast::statement::restricted_block* top_level_block_stat)
{
	out << "digraph\n{\n";

	write_node(top_level_block_stat, "top level block");
	auto o_parent_id = parent_id;
	parent_id = top_level_block_stat;

	top_level_block_stat->visit_children(this);

	parent_id = o_parent_id;

	out << "}\n";
	return false;
}

bool graphvizitor::visit(ir::ast::statement::ret* ret_stat)
{
	write_node(ret_stat, "return");
	auto o_parent_id = parent_id;
	parent_id = ret_stat;

	ret_stat->visit_children(this);

	parent_id = o_parent_id;
	return false;
}
