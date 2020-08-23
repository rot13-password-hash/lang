#pragma once

#include "../ir/ast/ast.h"
#include "../lexer/lexer.h"
#include "../utils/position.h"

#include <llvm/Support/Error.h>

#include <string_view>
#include <memory>
#include <unordered_map>

namespace seam::compiler::parser
{
	class parser
	{
		lexer::lexer lexer;

		llvm::Error expect(lexer::lexeme::lexeme_type type, bool should_consume = false);

		llvm::Expected<ir::ast::type> parse_type();
		llvm::Expected<ir::ast::var> parse_var();
		
		llvm::Expected<std::vector<ir::ast::var>> parse_var_list();
		llvm::Expected<std::vector<std::unique_ptr<ir::ast::expression::expression>>> parse_expr_list();
		
		llvm::Expected<std::unique_ptr<ir::ast::expression::call>> parse_call_expr_args(std::unique_ptr<ir::ast::expression::expression> func);
		llvm::Expected<std::unique_ptr<ir::ast::expression::expression>> parse_expr();
		llvm::Expected<std::unique_ptr<ir::ast::expression::expression>> parse_prefix_expr();

		llvm::Expected<std::unique_ptr<ir::ast::statement::function_definition>> parse_function_definition_stat();
		llvm::Expected<std::unique_ptr<ir::ast::statement::type_definition>> parse_type_definition_stat();
		llvm::Expected<std::unique_ptr<ir::ast::statement::ret>> parse_return_stat();
		llvm::Expected<std::unique_ptr<ir::ast::statement::block>> parse_block_stat();
		llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_statement>> parse_restricted_stat();
		llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_block>> parse_block_restricted_stat();

	public:
		explicit parser(std::string_view source) :
			lexer(source)
		{}

		llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_block>> parse();
	};
}