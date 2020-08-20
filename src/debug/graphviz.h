#pragma once
#include <string>
#include <sstream>

#include "../compiler/ir/ast/ast.h"

class graphvizitor : public lang::compiler::ir::ast::visitor
{
	std::stringstream out;

	void* parent_id = nullptr;
	void write_node(void* id, const std::string& label);

	static std::string format_type(const lang::compiler::ir::ast::type_reference& type_ref);
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
	bool visit(lang::compiler::ir::ast::expression::unresolved_variable* unresolved_var_expr) override;

	bool visit(lang::compiler::ir::ast::statement::function_definition* func_def_stat) override;
	bool visit(lang::compiler::ir::ast::statement::alias_type_definition* alias_type_def) override;
	bool visit(lang::compiler::ir::ast::statement::class_type_definition* class_type_def) override;

	bool visit(lang::compiler::ir::ast::statement::block* block_stat) override;
	bool visit(lang::compiler::ir::ast::statement::restricted_block* top_level_block_stat) override;
	bool visit(lang::compiler::ir::ast::statement::ret* ret_stat) override;

	std::string string() { return out.str(); }
};