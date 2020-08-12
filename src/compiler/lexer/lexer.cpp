#include "lexer.h"

namespace lang::compiler::lexer
{
	char lexer::peek_character(std::size_t offset) const
	{
		return read_offset + offset >= source.length() ? eof : source[read_offset + offset];
	}

	void lexer::consume_character()
	{
		++read_offset;
	}

	void lexer::skip_comment(bool long_comment)
	{
		
	}
	
	lexer::lexer(const std::string_view& source)
		: source(source) {}

	const lexeme& lexer::next_lexeme()
	{
		switch (peek_character())
		{
			case '/':
			{
				consume_character();
				const auto next_character = peek_character();
					
				const auto is_long = next_character == '*';

				if (!is_long && next_character != '/')
				{
					// not a comment
				}

				skip_comment(is_long); // if next_character is asterisk, then it's a long comment
				break;
			}
		}
	}

}