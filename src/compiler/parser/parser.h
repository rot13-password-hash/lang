#pragma once

#include "../ir/ast.h"
#include "../lexer/lexer.h"

#include <string_view>
#include <memory>

namespace lang::compiler::parser
{
	class parser
	{
		lexer::lexer lexer;

		ir::ast::position current_position();
	public:
		explicit parser(const std::string_view& source) :
			lexer(source) {}

		std::unique_ptr<ir::ast::statement::block> parse();
	};
}