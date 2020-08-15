#include "parser.h"
#include "../utils/exception.h"

#include <iostream>
#include <sstream>

#include "../../bytecode/instruction.h"

#include "passes/pass.h"

using namespace lang::compiler;
using lexeme_type = lexer::lexeme::lexeme_type;

void parser::parser::expect(lexeme_type type, bool should_consume)
{
	if (lexer.current_lexeme().type != type)
	{
		std::stringstream error_message;
		error_message << "expected " << lexer::lexeme::to_string(type) << ", got " << lexer.current_lexeme().to_string();
		throw exception{ lexer.current_lexeme().pos, error_message.str() };
	}

	if (should_consume)
	{
		lexer.next_lexeme();
	}
}

ir::ast::type parser::parser::parse_type()
{
	expect(lexeme_type::identifier);
	const auto target_type_name = std::string{ lexer.current_lexeme().value };
	lexer.next_lexeme();

	if (lexer.current_lexeme().type == lexer::lexeme::lexeme_type::symb_question)
	{
		lexer.next_lexeme();

		return { std::move(target_type_name), true };
	}

	return { std::move(target_type_name), false };
}

std::vector<ir::ast::var> parser::parser::parse_var_list()
{
	return {};
}

std::vector<std::unique_ptr<ir::ast::expression::expression>> parser::parser::parse_expr_list()
{
	std::vector<std::unique_ptr<ir::ast::expression::expression>> list;

	list.push_back(parse_expr());

	while (lexer.current_lexeme().type == lexeme_type::symb_comma)
	{
		lexer.next_lexeme();
		list.push_back(parse_expr());
	}

	return list;
}


std::unique_ptr<ir::ast::expression::call> parser::parser::parse_call_expr_args(std::unique_ptr<ir::ast::expression::expression> func)
{
	const auto start = lexer.current_lexeme().pos;

	lexer.next_lexeme();
	auto arguments = parse_expr_list();
	expect(lexeme_type::symb_close_parenthesis, true);

	return std::make_unique<ir::ast::expression::call>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(func), std::move(arguments));
}

std::unique_ptr<ir::ast::expression::expression> parser::parser::parse_expr()
{	
	const auto start = lexer.current_lexeme().pos;
	switch (const auto type = lexer.current_lexeme().type)
	{
		case lexeme_type::kw_true:
		case lexeme_type::kw_false:
		{
			lexer.next_lexeme();
			return std::make_unique<ir::ast::expression::literal<bool>>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, type != lexeme_type::kw_false);
		}
		case lexeme_type::number_literal:
		{
			lexer.next_lexeme();
			return std::make_unique<ir::ast::expression::literal<ir::ast::number>>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, ir::ast::number{ std::string{ lexer.current_lexeme().value } } );
		}
		case lexeme_type::string_literal:
		{
			lexer.next_lexeme();
			return std::make_unique<ir::ast::expression::literal<std::string>>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::string{ lexer.current_lexeme().value });
		}
		case lexeme_type::symb_open_parenthesis:
		case lexeme_type::identifier:
		{
			auto expr = parse_prefix_expr();
			while (lexer.current_lexeme().pos.line == start.line)
			{
				switch (lexer.current_lexeme().type)
				{
					case lexeme_type::symb_open_parenthesis:
					{
						expr = parse_call_expr_args(std::move(expr));
						continue;
					}
				}

				break;
			}
			return expr;
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected expression, got " << lexer.current_lexeme().to_string();
			throw exception{ start, error_message.str() };
		}
	}
}

std::unique_ptr<ir::ast::expression::expression> parser::parser::parse_prefix_expr()
{
	const auto start = lexer.current_lexeme().pos;

	switch (lexer.current_lexeme().type)
	{
		case lexeme_type::symb_open_parenthesis:
		{
			lexer.next_lexeme();
			auto expr = parse_expr();
			expect(lexeme_type::symb_close_parenthesis, true);
			return expr;
		}
		case lexeme_type::identifier:
		{
			auto identifier_name = std::string{ lexer.current_lexeme().value };
			lexer.next_lexeme();
			return std::make_unique<ir::ast::expression::unresolved_variable>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(identifier_name)); // TODO: resolve local variables
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '(' or identifier, got " << lexer.current_lexeme().to_string();
			throw exception{ lexer.current_lexeme().pos, error_message.str() };
		}
	}
}

std::unique_ptr<ir::ast::statement::function_definition> parser::parser::parse_function_definition_stat()
{
	position start = lexer.current_lexeme().pos;
	lexer.next_lexeme();
	expect(lexeme_type::identifier);

	// store function name
	const auto function_name = lexer.current_lexeme().value;
	lexer.next_lexeme();

	expect(lexeme_type::symb_open_parenthesis, true);
	auto arg_list = parse_var_list();
	expect(lexeme_type::symb_close_parenthesis, true);

	ir::ast::type return_type {};
	if (lexer.current_lexeme().type == lexeme_type::symb_arrow) // return type
	{
		lexer.next_lexeme();
		return_type = parse_type();
	}
	else
	{
		return_type.name = "void";
		return_type.is_optional = false;
	}

	expect(lexeme_type::symb_open_brace);
	auto block = parse_block_stat();

	return std::make_unique<ir::ast::statement::function_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::string{ function_name }, std::move(arg_list),
		std::move(return_type), std::move(block));
}

std::unique_ptr<ir::ast::statement::type_definition> parser::parser::parse_type_definition_stat()
{
	lexer.next_lexeme();
	expect(lexeme_type::identifier);

	const auto start = lexer.current_lexeme().pos;
	const auto type_name = std::string{ lexer.current_lexeme().value };

	lexer.next_lexeme();

	switch (lexer.current_lexeme().type)
	{
		case lexeme_type::symb_equals:
		{
			lexer.next_lexeme();

			auto target_type_desc = parse_type();
			return std::make_unique<ir::ast::statement::alias_type_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos },
				type_name, target_type_desc);
		}
		case lexeme_type::symb_open_brace:
		{
			//type_map[identifier_string] = std::make_unique<user_defined_type_desc>(identifier_string);
			lexer.next_lexeme();

			while (lexer.current_lexeme().type != lexeme_type::symb_close_brace)
			{
				expect(lexeme_type::identifier);
				const auto& field_name = lexer.current_lexeme();
				lexer.next_lexeme();

				expect(lexeme_type::symb_colon, true);

				const auto type = parse_type();
				// TODO: construct node
			}

			lexer.next_lexeme();
			break;
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '=' or '{', got " << lexer.current_lexeme().to_string();
			throw exception{ lexer.current_lexeme().pos, error_message.str() };
		}
	}
}

std::unique_ptr<ir::ast::statement::ret> parser::parser::parse_return_stat()
{
	const auto start = lexer.current_lexeme().pos;
	lexer.next_lexeme();

	std::unique_ptr<ir::ast::expression::expression> expr = nullptr;
	if (lexer.current_lexeme().type != lexeme_type::symb_close_brace
		&& lexer.current_lexeme().pos.line == start.line)
	{
		expr = parse_expr();
	}
	return std::make_unique<ir::ast::statement::ret>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(expr));
}

std::unique_ptr<ir::ast::statement::block> parser::parser::parse_block_stat()
{
	position start = lexer.current_lexeme().pos;
	lexer.next_lexeme();

	std::vector<std::unique_ptr<ir::ast::statement::statement>> body;
	while (true)
	{
		const auto& lexeme = lexer.current_lexeme();
		if (lexeme.type == lexeme_type::symb_close_brace)
		{
			lexer.next_lexeme();
			break;
		}

		switch (lexeme.type)
		{
			case lexeme_type::kw_return:
			{
				body.push_back(parse_return_stat());
				break;
			}
		}
	}

	return std::make_unique<ir::ast::statement::block>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(body));
}

std::unique_ptr<ir::ast::statement::top_level_block> parser::parser::parse_block_global_restricted_stat()
{
	position start = lexer.current_lexeme().pos;
	
	std::vector<std::unique_ptr<ir::ast::statement::restricted_statement>> body;
	while (true)
	{
		const auto& lexeme = lexer.current_lexeme();

		if (lexeme.type == lexeme_type::eof)
		{
			break;
		}

		switch (lexeme.type)
		{
			case lexeme_type::kw_fn:
			{
				body.push_back(parse_function_definition_stat());
				break;
			}
			case lexeme_type::kw_type:
			{
				body.push_back(parse_type_definition_stat());
				break;
			}
			default:
			{
				std::stringstream error_message;
				error_message << "unexpected identifier " << lexeme.to_string() << " in restricted (global) namespace, expected a type or function definition";
				throw exception{ lexer.current_lexeme().pos, error_message.str() };
			}
		}
	}

	return std::make_unique<ir::ast::statement::top_level_block>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(body));
}

std::unique_ptr<ir::ast::statement::top_level_block> parser::parser::parse()
{
	lexer.next_lexeme();

	auto root = parse_block_global_restricted_stat();
	pass::invoke_all(root.get());

	return std::move(root);
}