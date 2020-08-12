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
			identifier,
			
			/* literals */
			string_literal,

			/* keywords */
			kw_function,
			kw_type,			
			
			/* symbols */
			symb_add,
			symb_add_assign,
			symb_minus,
			symb_minus_assign,
			symb_divide,
			symb_divide_assign,
		};
		
		lexeme_type type = lexeme_type::eof;


		std::variant<std::monostate, std::string_view, double> value;

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
				case lexeme_type::identifier:
				{
					return "<identifier>";
				}
				case lexeme_type::kw_function:
				{
					return "fn";
				}
				case lexeme_type::kw_type:
				{
					return "type";
				}
				case lexeme_type::string_literal:
				{
					return "<string>";
				}
				case lexeme_type::symb_add:
				{
					return "+";
				}
				case lexeme_type::symb_add_assign:
				{
					return "+=";
				}
				case lexeme_type::symb_minus:
				{
					return "-";
				}
				case lexeme_type::symb_minus_assign:
				{
					return "-=";
				}
				case lexeme_type::symb_divide:
				{
					return "/";
				}
				case lexeme_type::symb_divide_assign:
				{
					return "/=";
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
				case lexeme_type::identifier:
				{
					const auto& view = std::get<std::string_view>(value);
					return { view.cbegin(), view.cend() };
				}
				case lexeme_type::string_literal:
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