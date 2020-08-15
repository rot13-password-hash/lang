#pragma once
#include <string>
#include <sstream>

#include "../compiler/ir/ast.h"

class graphvizitor : public lang::compiler::ir::ast::visitor
{
	std::stringstream out;

	void* parent_id = nullptr;
	void write_node(void* id, const std::string& label);
public:
	bool visit(lang::compiler::ir::ast::expression::literal<std::string>* literal_string_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<lang::compiler::ir::ast::number>* literal_number_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::int8_t>* literal_i8_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::int16_t>* literal_i16_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::int32_t>* literal_i32_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::int64_t>* literal_i64_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::uint8_t>* literal_u8_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::uint16_t>* literal_u16_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::uint32_t>* literal_u32_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<std::uint64_t>* literal_u64_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<float>* literal_f32_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<double>* literal_f64_expr) override;
	bool visit(lang::compiler::ir::ast::expression::literal<bool>* literal_bool_expr) override;

	bool visit(lang::compiler::ir::ast::expression::call* call_expr) override;
};