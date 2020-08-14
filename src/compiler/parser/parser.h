#pragma once

#include "../ir/ast.h"
#include "../lexer/lexer.h"
#include "../utils/position.h"

#include <string_view>
#include <memory>
#include <unordered_map>

namespace lang::compiler::parser
{
	class parser
	{
		lexer::lexer lexer;

		void expect(lexer::lexeme::lexeme_type type, bool should_consume = false);

		std::string parse_type();

		std::vector<ir::ast::var> parse_var_list();

		std::unique_ptr<ir::ast::expression::expression> parse_expr();

		std::unique_ptr<ir::ast::statement::function_definition> parse_function_definition_stat();
		std::unique_ptr<ir::ast::statement::type_definition> parse_type_definition_stat();
		std::unique_ptr<ir::ast::statement::ret> parse_return_stat();
		std::unique_ptr<ir::ast::statement::block> parse_block_stat();
		std::unique_ptr<ir::ast::statement::top_level_block> parse_block_global_restricted_stat();

	public:
		explicit parser(std::string_view source) :
			lexer(source)
		{}

		std::unique_ptr<ir::ast::statement::top_level_block> parse();
	};
}