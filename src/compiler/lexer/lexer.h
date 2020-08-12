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
		std::size_t line = 1;
		std::size_t column = 1;

		lexeme current;

		
		char peek_character(std::size_t offset = 0) const;
		void consume_character();

		void skip_comment();
		void lex_string();
		void lex_keyword_or_id();
		void lex_symbol();
	public:
		explicit lexer(const std::string_view& source);

		lexeme& current_lexeme() { return current; }
		const lexeme& next_lexeme();
		
		[[nodiscard]] std::size_t get_line() const { return line; }
		[[nodiscard]] std::size_t get_column() const { return column; }
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