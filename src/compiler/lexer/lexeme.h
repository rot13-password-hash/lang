#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace lang::compiler::lexer
{
	struct lexeme
	{
		enum class lexeme_type
		{
			eof,

			reserved_and,
			reserved_break,
			reserved_do,
			reserved_else,
			reserved_elseif,
			reserved_end,
			reserved_false,
			reserved_for,
			reserved_function,
			reserved_if,
			reserved_in,
			reserved_local,
			reserved_nil,
			reserved_not,
			reserved_or,
			reserved_repeat,
			reserved_return,
			reserved_then,
			reserved_true,
			reserved_until,
			reserved_while,

			name,
			number,
			string,

			open_parenthesis,
			close_parenthesis,
			open_bracket,
			close_bracket,
			open_brace,
			close_brace
		};

		lexeme_type type = lexeme_type::eof;


		std::variant<std::string_view, double> value;

		std::size_t line = 0;
		std::size_t col = 0;

		static std::string to_string(lexeme_type type)
		{
			switch (type)
			{
			case lexeme_type::eof:
			{
				return "<eof>";
			}
			case lexeme_type::reserved_and:
			{
				return "'and'";
			}
			case lexeme_type::reserved_break:
			{
				return "'break'";
			}
			case lexeme_type::reserved_do:
			{
				return "'do'";
			}
			case lexeme_type::reserved_else:
			{
				return "'else'";
			}
			case lexeme_type::reserved_elseif:
			{
				return "'elseif'";
			}
			case lexeme_type::reserved_end:
			{
				return "'end'";
			}
			case lexeme_type::reserved_false:
			{
				return "'false'";
			}
			case lexeme_type::reserved_for:
			{
				return "'for'";
			}
			case lexeme_type::reserved_function:
			{
				return "'function'";
			}
			case lexeme_type::reserved_if:
			{
				return "'if'";
			}
			case lexeme_type::reserved_in:
			{
				return "'in'";
			}
			case lexeme_type::reserved_local:
			{
				return "'local'";
			}
			case lexeme_type::reserved_nil:
			{
				return "'nil'";
			}
			case lexeme_type::reserved_not:
			{
				return "'not'";
			}
			case lexeme_type::reserved_or:
			{
				return "'or'";
			}
			case lexeme_type::reserved_repeat:
			{
				return "'repeat'";
			}
			case lexeme_type::reserved_return:
			{
				return "'return'";
			}
			case lexeme_type::reserved_then:
			{
				return "'then'";
			}
			case lexeme_type::reserved_true:
			{
				return "'true'";
			}
			case lexeme_type::reserved_until:
			{
				return "'until'";
			}
			case lexeme_type::reserved_while:
			{
				return "'while'";
			}
			case lexeme_type::name:
			{
				return "<name>";
			}
			case lexeme_type::string:
			{
				return "<string>";
			}
			default:
			{
				return "<unknown>";
			}
			}
		}

		std::string to_string()
		{
			switch (type)
			{
			case lexeme_type::name:
			{
				const auto& view = std::get<std::string_view>(value);
				return { view.cbegin(), view.cend() };
			}
			case lexeme_type::string:
			{
				const auto& view = std::get<std::string_view>(value);
				return '"' + std::string{ view.cbegin(), view.cend() } +'"';
			}
			default:
			{
				return to_string(type);
			}
			}
		}
	};
}