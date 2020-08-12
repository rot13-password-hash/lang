#pragma once

#include "lexeme.h"

#include <string_view>
#include <cstddef>

namespace lang::compiler::lexer
{
	class lexer
	{
		constexpr static char eof = -1;
		
		std::string_view source;

		std::size_t read_offset = 0;
		std::size_t line = 0;
		std::size_t column = 0;

		char peek_character(std::size_t offset = 0) const;
		void consume_character();

		void skip_comment(bool long_comment = false);
	public:
		explicit lexer(const std::string_view& source);
		
		const lexeme& current_lexeme();
		const lexeme& next_lexeme();
	};
	
	class lexer_
	{
		std::string scratch;

		std::string_view source;
		std::size_t offset = 0;
		std::size_t line = 0;
		std::size_t line_offset = 0;

		lexeme current_lexeme;

		void consume_newline();
		char peek(std::size_t off = 0);
		void consume_comment();
		void consume_whitespace();
		void consume();

		// returns true if all characters are uppercase
		bool read_name(std::string_view& view);
		char read_escaped_char();
		std::string_view read_string(char delim);
	public:
		const lexeme& next();
		const lexeme& current();

		explicit lexer_(const std::string_view& source) :
			source(source) {}
	};
}