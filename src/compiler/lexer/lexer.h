#pragma once

#include "lexeme.h"
#include "../utils/position.h"

#include <string_view>
#include <cstddef>

namespace seam::compiler::lexer
{
	class lexer
	{
		constexpr static char eof_char = -1;
		
		std::string_view source;

		std::size_t read_offset = 0;
		std::size_t line = 1;
		std::size_t line_start_offset = 0;

		lexeme current;

        position current_position() const;
		
		char peek_character(std::size_t offset = 0) const;
		void consume_character();

		void skip_whitespace();
		void skip_comment();
		void lex_string();
		void lex_keyword_or_id();
		void lex_symbol();
		void lex_number_literal();
	public:
		explicit lexer(const std::string_view& source);

		[[nodiscard]] const lexeme& current_lexeme() const { return current; }
		void next_lexeme();
	};
}