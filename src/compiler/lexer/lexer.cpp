#include "lexer.h"

#include <map>

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
	
	const std::map<std::string, lexeme::lexeme_type> symbol_map
	{
		{ "+", lexeme::lexeme_type::symb_add },
		{ "+=", lexeme::lexeme_type::symb_add_assign },
		{ "-", lexeme::lexeme_type::symb_minus },
		{ "-=", lexeme::lexeme_type::symb_minus_assign },
	};

	const std::map<std::string, lexeme::lexeme_type> keyword_map
	{
		{ "fn", lexeme::lexeme_type::kw_function },
		{ "type", lexeme::lexeme_type::kw_type },
	};
	
	char lexer::peek_character(const std::size_t offset) const
	{
		return read_offset + offset >= source.length() ? eof : source[read_offset + offset];
	}

	void lexer::consume_character()
	{
		if (peek_character() == '\n')
		{
			++line;
		}
		++column;
		++read_offset;
	}

	void lexer::skip_comment()
	{
		const auto is_long_comment = peek_character() == '/';
		while (true)
		{			
			const auto peeked_character = peek_character();
			if (!is_long_comment && peeked_character == '\n')
			{
				consume_character();
				return;
			}

			if (is_long_comment && peeked_character == '/' && peek_character(1) == '/' && peek_character(2) == '/') // End of long comment
			{
				consume_character();
				consume_character();
				consume_character();
				return;
			}
		}
	}

	void lexer::lex_string()
	{
		std::string value;
		
		while (true)
		{
			if (peek_character() == '"')
			{
				consume_character();
				break;
			}

			if (peek_character() == '\n')
			{
				// Error: Unterminated String Literal
				break;
			}
			
			value += peek_character();
			consume_character();
		}
		
		current.type = lexeme::lexeme_type::string_literal;
		current.value = value;
	}

	void lexer::lex_keyword_or_id()
	{
		auto can_be_keyword = is_keyword_char(peek_character());
		std::string value;

		value += peek_character();
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
				current = {
					lexeme::lexeme_type::identifier,
					value
				};
				break;
			}
			
			value += next_char;
			consume_character();
		}

		if (can_be_keyword)
		{
			if (const auto keyword_type = keyword_map.find(value); keyword_type != keyword_map.end())
			{
				current.type = keyword_type->second;

				if (current.type == lexeme::lexeme_type::kw_type)
				{
					std::string type_name;

					if (!std::isspace(peek_character()))
					{
						return; // error: bad parse
					}
					consume_character();
					
					while (true)
					{
						if (type_name.length() == 0 && !is_start_identifier_char(peek_character()))
						{
							return; // error: identifier cannot 
						}

						if (!is_identifier_char(peek_character()))
						{
							consume_character();
							break;
						}

						type_name += peek_character();
						consume_character();
					}

					current.value = type_name;
				}
			}
		}
	}

	void lexer::lex_symbol()
	{
		std::string long_symbol;
		long_symbol += peek_character();

		consume_character();
		if (const auto symbol = symbol_map.find(long_symbol + peek_character(1)); symbol != symbol_map.end())
		{
			consume_character();
			current.type = symbol->second;
		}
		else if (const auto symbol = symbol_map.find(long_symbol); symbol != symbol_map.end())
		{
			current.type = symbol->second;
		}
	}

	
	lexer::lexer(const std::string_view& source)
		: source(source) {}

	const lexeme& lexer::next_lexeme()
	{
		current.value = std::monostate(); // clear every time to be safe
		switch (peek_character())
		{
			case '/':
			{
				consume_character();
				const auto next_character = peek_character();
					

				if (next_character == '/')
				{
					consume_character();
					skip_comment(); // if next_character is asterisk, then it's a long comment
					return next_lexeme();
				}

					
				if (next_character == '=')
				{
					current = {
						lexeme::lexeme_type::symb_divide_assign
					};
				}
				else
				{
					current = {
						lexeme::lexeme_type::symb_divide
					};
				}
			}
			case '"':
			{
				consume_character();
				lex_string();
				break;
			}
			default:
			{
				/* identifier + keywords + symbols */
				if (is_start_identifier_char(peek_character())) // must be keyword or identifier
				{
					lex_keyword_or_id();
					break;
				}

				if (std::ispunct(peek_character())) // must be a symbol
				{
					break;
				}
				// Unknown Symbol
				return; // error
			}
		}
		return current;
	}

}
