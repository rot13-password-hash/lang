#include "lexer.h"
#include "../utils/exception.h"

#include <unordered_map>
#include <iostream>

namespace lang::compiler::lexer
{
	bool is_start_identifier_char(const char value)
	{
		return std::isalpha(value) || value == '_';
	}

	bool is_identifier_char(const char value)
	{
		return std::isalnum(value) || value == '_';
	}

	bool is_keyword_char(const char value)
	{
		return value != '_' && !std::isupper(value);
	}

	using lexeme_map_t = std::unordered_map<std::string_view, lexeme::lexeme_type>;
	
	const lexeme_map_t symbol_map
	{
		{ "+", lexeme::lexeme_type::symb_add },
		{ "+=", lexeme::lexeme_type::symb_add_assign },
		{ "-", lexeme::lexeme_type::symb_minus },
		{ "-=", lexeme::lexeme_type::symb_minus_assign },
		{ "*", lexeme::lexeme_type::symb_multiply },
		{ "*=", lexeme::lexeme_type::symb_multiply_assign },
		{ "(", lexeme::lexeme_type::symb_open_parenthesis },
		{ ")", lexeme::lexeme_type::symb_close_parenthesis },
		{ "[", lexeme::lexeme_type::symb_open_bracket },
		{ "]", lexeme::lexeme_type::symb_close_bracket },
		{ "{", lexeme::lexeme_type::symb_open_brace },
		{ "}", lexeme::lexeme_type::symb_close_brace },
		{ "->", lexeme::lexeme_type::symb_arrow },
		{ "=", lexeme::lexeme_type::symb_equals },
		{ "?", lexeme::lexeme_type::symb_question },
		{ ":", lexeme::lexeme_type::symb_colon },
		{ ",", lexeme::lexeme_type::symb_comma },
	};

	const lexeme_map_t keyword_map
	{
		{ "fn", lexeme::lexeme_type::kw_fn },
		{ "as", lexeme::lexeme_type::kw_as },
		{ "return", lexeme::lexeme_type::kw_return },
		{ "type", lexeme::lexeme_type::kw_type },
		{ "try", lexeme::lexeme_type::kw_try },
		{ "catch", lexeme::lexeme_type::kw_catch },
		{ "switch", lexeme::lexeme_type::kw_switch },
		{ "throw", lexeme::lexeme_type::kw_throw },
		{ "true", lexeme::lexeme_type::kw_true },
		{ "false", lexeme::lexeme_type::kw_false },
	};

	position lexer::current_position() const
	{
		return { line, read_offset - line_start_offset };
	}
	
	char lexer::peek_character(std::size_t offset) const
	{
		return read_offset + offset >= source.length() ? eof_char : source[read_offset + offset];
	}

	void lexer::consume_character()
	{
		if (peek_character() == '\n')
		{
			++line;
			line_start_offset = read_offset;
		}
		++read_offset;
	}

	void lexer::skip_whitespace()
	{
		char next_char;
		while ((next_char = peek_character()) != eof_char && std::isspace(next_char))
		{
			consume_character();
		}
	}

	void lexer::skip_comment()
	{
		const auto is_long_comment = peek_character() == '/';
		while (true)
		{			
			const auto next_char = peek_character();
			if (!is_long_comment && next_char == '\n')
			{
				consume_character();
				return;
			}

			if (is_long_comment && next_char == '/' && peek_character(1) == '/' && peek_character(2) == '/')
			{
				consume_character();
				consume_character();
				consume_character();
				return;
			}

			consume_character();
		}
	}

	void lexer::lex_string()
	{
		current.type = lexeme::lexeme_type::string_literal;

		std::size_t start_offset = read_offset;
		while (true)
		{
			if (peek_character() == '\\' && peek_character(1) == '"')
			{
				consume_character();
				consume_character();
				continue;
			}
			else if (peek_character() == '"')
			{
				current.value = source.substr(start_offset, read_offset - start_offset);
				consume_character();
				return;
			}
			
			consume_character();
		}
	}

	void lexer::lex_keyword_or_id()
	{
		auto can_be_keyword = is_keyword_char(peek_character());
		
		const auto start_offset = read_offset;
		consume_character();
		
		while (true)
		{
			const auto next_char = peek_character();
			
			if (!is_keyword_char(next_char))
			{
				can_be_keyword = false;
			}
			
			if (!is_identifier_char(next_char))
			{
				current.type = lexeme::lexeme_type::identifier;
				current.value = source.substr(start_offset, read_offset - start_offset);
				break;
			}

			consume_character();
		}

		if (can_be_keyword)
		{
			if (const auto keyword_type = keyword_map.find(source.substr(start_offset, read_offset - start_offset)); keyword_type != keyword_map.cend())
			{
				current.type = keyword_type->second;
			}
		}
	}

	void lexer::lex_symbol()
	{
		// TODO: REWRITE THIS TRASH
		auto start_offset = read_offset;

		consume_character();
		lexeme_map_t::const_iterator symbol;
		if (peek_character(1) != eof_char && (symbol = symbol_map.find(source.substr(start_offset, 2))) != symbol_map.cend())
		{
			consume_character();
			current.type = symbol->second;
		}
		else if (symbol = symbol_map.find(source.substr(start_offset, 1)); symbol != symbol_map.cend())
		{
			current.type = symbol->second;
		}
		else
		{
			throw compiler::exception{
				current_position(),
				"unexpected symbol"
			};
		}
	}

	void lexer::lex_number_literal()
	{
		const auto is_hex = peek_character() == '0' && peek_character(1) == 'x';
		auto is_float = false;

		const auto start_offset = read_offset;

		if (is_hex)
		{
			consume_character();
			consume_character(); // consume 0x
		}

		while (true)
		{
			if (peek_character() == '.')
			{
				is_float = true;
				consume_character();
			}
			
			if (is_hex && is_float)
			{
				throw compiler::exception {
					current_position(),
					"malformed number"
				}; 
			}

			auto next_char = peek_character();
			if ((is_hex ? std::isxdigit(next_char) : std::isdigit(next_char)) || next_char == '_')
			{
				consume_character();
				next_char = peek_character();
			}
			else
			{
				break;
			}
		}

		current.value = source.substr(start_offset, read_offset - start_offset);
		current.type = lexeme::lexeme_type::number_literal;
	}

	
	lexer::lexer(const std::string_view& source)
		: source(source) {}

	void lexer::next_lexeme()
	{
		skip_whitespace();

		current.pos = current_position();

		switch (peek_character())
		{
			case eof_char:
			{
				current.type = lexeme::lexeme_type::eof;
				return;
			}
			case '/':
			{
				consume_character();
				const auto next_char = peek_character();
					

				if (next_char == '/')
				{
					consume_character();
					skip_comment();
					return next_lexeme();
				}

				if (next_char == '=')
				{
					consume_character();
					current.type = lexeme::lexeme_type::symb_divide_assign;
				}
				else
				{
					current.type = lexeme::lexeme_type::symb_divide;
				}

				return;
			}
			case '"':
			{
				consume_character();
				lex_string();
				return;
			}
			case '@':
			{
				consume_character();

				const auto start_offset = read_offset;

				if (!is_start_identifier_char(peek_character()))
				{
					throw compiler::exception{
						current_position(),
						"unexpected symbol"
					};
				}

				consume_character();

				while (true)
				{
					const auto next_char = peek_character();

					if (!is_identifier_char(next_char))
					{
						current.type = lexeme::lexeme_type::attribute;
						current.value = source.substr(start_offset, read_offset - start_offset);
						break;
					}

					consume_character();
				}
				return;
			}
			default:
			{
				/* identifier + keywords + symbols */
				if (is_start_identifier_char(peek_character())) // must be keyword or identifier
				{
					lex_keyword_or_id();
					return;
				}

				if (std::isdigit(peek_character()))
				{
					lex_number_literal();					
					return;
				}

				lex_symbol();
			}
		}
	}

}
