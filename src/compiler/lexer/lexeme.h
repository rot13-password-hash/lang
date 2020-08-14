#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "../utils/position.h"

namespace lang::compiler::lexer
{
	struct lexeme
	{
		enum class lexeme_type
		{
			eof,
			identifier,
			
			// literals
			string_literal,
			number_literal,

			// keywords
			kw_fn,
			kw_return,
			kw_type,
			kw_try,
			kw_catch,
			kw_switch,
			kw_throw,
			kw_true,
			kw_false,
			
			// symbols
			symb_add,
			symb_add_assign,
			symb_minus,
			symb_minus_assign,
			symb_multiply,
			symb_multiply_assign,
			symb_divide,
			symb_divide_assign,
			symb_arrow,
			symb_equals,
			symb_question,
			symb_colon,

			// symbol pairs
			symb_open_parenthesis,
			symb_close_parenthesis,
			symb_open_bracket,
			symb_close_bracket,
			symb_open_brace,
			symb_close_brace,
		};
		
		lexeme_type type = lexeme_type::eof;

		std::string_view value;

        position pos{ 0, 0 };

		static std::string to_string(lexeme_type type)
		{
			switch (type)
			{
				case lexeme_type::eof:
				{
					return "eof";
				}
				case lexeme_type::identifier:
				{
					return "identifier";
				}
				case lexeme_type::kw_fn:
				{
					return "'fn'";
				}
				case lexeme_type::kw_return:
				{
					return "'return'";
				}
				case lexeme_type::kw_type:
				{
					return "'type'";
				}
				case lexeme_type::kw_try:
				{
					return "'try'";
				}
				case lexeme_type::kw_catch:
				{
					return "'catch'";
				}
				case lexeme_type::kw_switch:
				{
					return "'switch'";
				}
				case lexeme_type::kw_throw:
				{
					return "'throw'";
				}
				case lexeme_type::kw_true:
				{
					return "true";
				}
				case lexeme_type::kw_false:
				{
					return "false";
				}
				case lexeme_type::string_literal:
				{
					return "<string>";
				}
				case lexeme_type::symb_add:
				{
					return "'+'";
				}
				case lexeme_type::symb_add_assign:
				{
					return "'+='";
				}
				case lexeme_type::symb_minus:
				{
					return "'-'";
				}
				case lexeme_type::symb_minus_assign:
				{
					return "'-='";
				}
				case lexeme_type::symb_multiply:
				{
					return "'*'";
				}
				case lexeme_type::symb_multiply_assign:
				{
					return "'*='";
				}
				case lexeme_type::symb_divide:
				{
					return "'/'";
				}
				case lexeme_type::symb_divide_assign:
				{
					return "'/='";
				}
				case lexeme_type::symb_arrow:
				{
					return "'->'";
				}
				case lexeme_type::symb_equals:
				{
					return "'='";
				}
				case lexeme_type::symb_question:
				{
					return "'?'";
				}
				case lexeme_type::symb_colon:
				{
					return "':'";
				}
				case lexeme_type::symb_open_parenthesis:
				{
					return "'('";
				}
				case lexeme_type::symb_close_parenthesis:
				{
					return "')'";
				}
				case lexeme_type::symb_open_bracket:
				{
					return "'['";
				}
				case lexeme_type::symb_close_bracket:
				{
					return "']'";
				}
				case lexeme_type::symb_open_brace:
				{
					return "'{'";
				}
				case lexeme_type::symb_close_brace:
				{
					return "'}'";
				}
				default:
				{
					return "<unknown>";
				}
			}
		}

		std::string to_string() const
		{
			switch (type)
			{
				case lexeme_type::number_literal:
				case lexeme_type::identifier:
				{
					return { value.cbegin(), value.cend() };
				}
				case lexeme_type::string_literal:
				{
					return '"' + std::string{ value.cbegin(), value.cend() } +'"';
				}
				default:
				{
					return to_string(type);
				}
			}
		}
	};
}