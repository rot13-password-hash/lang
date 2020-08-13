#pragma once

#include "../ir/ast.h"
#include "../lexer/lexer.h"
#include "../utils/position.h"

#include "types/types.h"
#include "types/value.h"

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
		std::unique_ptr<ir::ast::statement::function_definition> parse_function_definition();
		void parse_type_definition();

		std::unique_ptr<ir::ast::statement::block> parse_block();
		std::unique_ptr<ir::ast::statement::top_level_block> parse_block_global();

	public:
		explicit parser(std::string_view source) :
			lexer(source)
		{}

		std::unique_ptr<ir::ast::statement::top_level_block> parse();
	};
}