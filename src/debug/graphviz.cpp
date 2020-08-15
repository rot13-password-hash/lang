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
	write_node(literal_string_expr, literal_string_expr->val);
	return false;
}

bool graphvizitor::visit(ir::ast::expression::literal<ir::ast::number>* literal_number_expr)
{
	write_node(literal_number_expr, literal_number_expr->val.value);
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