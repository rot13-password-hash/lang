#include "parser.h"
#include "../utils/exception.h"
#include "../utils/error.h"

#include <iostream>
#include <sstream>

#include "passes/pass.h"

using namespace seam::compiler;
using lexeme_type = lexer::lexeme::lexeme_type;

llvm::Error parser::parser::expect(lexeme_type type, bool should_consume)
{
	if (lexer.current_lexeme().type != type)
	{
		std::stringstream error_message;
		error_message << "expected " << lexer::lexeme::to_string(type) << ", got " << lexer.current_lexeme().to_string();
		return llvm::make_error<error_info>(filename, lexer.current_lexeme().pos, error_message.str());
	}

	if (should_consume)
	{
		lexer.next_lexeme();
	}

	return llvm::Error::success();
}

llvm::Expected<ir::ast::type> parser::parser::parse_type()
{
	if (auto err = expect(lexeme_type::identifier))
	{
		return std::move(err);
	}

	auto target_type_name = std::string{ lexer.current_lexeme().value };
	lexer.next_lexeme();

	if (lexer.current_lexeme().type == lexer::lexeme::lexeme_type::symb_question)
	{
		lexer.next_lexeme();

		return ir::ast::type{ std::move(target_type_name), true };
	}

	return ir::ast::type{ std::move(target_type_name), false };
}

llvm::Expected<ir::ast::var> parser::parser::parse_var()
{
	const auto var_name = std::string{ lexer.current_lexeme().value };
	lexer.next_lexeme();

	if (auto err = expect(lexeme_type::symb_colon, true))
	{
		return std::move(err);
	}

	auto type = parse_type();
	if (!type)
	{
		return type.takeError();
	}

	return ir::ast::var{ *type, var_name };
}

llvm::Expected<std::vector<ir::ast::var>> parser::parser::parse_var_list()
{
	std::vector<ir::ast::var> var_list;

	if (lexer.current_lexeme().type != lexeme_type::symb_close_parenthesis)
	{
		auto first_var = parse_var();
		if (!first_var)
		{
			return first_var.takeError();
		}
		var_list.emplace_back(std::move(*first_var));

		while (lexer.current_lexeme().type == lexeme_type::symb_comma)
		{
			lexer.next_lexeme();

			auto var = parse_var();
			if (!var)
			{
				return var.takeError();
			}
			var_list.emplace_back(std::move(*var));
		}
	}
	
	return std::move(var_list);
}

llvm::Expected<std::vector<std::unique_ptr<ir::ast::expression::expression>>> parser::parser::parse_expr_list()
{
	std::vector<std::unique_ptr<ir::ast::expression::expression>> list;

	auto first_expr = parse_expr();
	if (!first_expr)
	{
		return first_expr.takeError();
	}

	list.push_back(std::move(*first_expr));

	while (lexer.current_lexeme().type == lexeme_type::symb_comma)
	{
		lexer.next_lexeme();
		// TODO: error recovery
		auto expr = parse_expr();
		if (!expr)
		{
			return expr.takeError();
		}

		list.push_back(std::move(*expr));
	}

	return list;
}


llvm::Expected<std::unique_ptr<ir::ast::expression::call>> parser::parser::parse_call_expr_args(std::unique_ptr<ir::ast::expression::expression> func)
{
	const auto start = lexer.current_lexeme().pos;

	std::vector<std::unique_ptr<ir::ast::expression::expression>> arguments;

	lexer.next_lexeme();
	if (lexer.current_lexeme().type != lexeme_type::symb_close_parenthesis)
	{
		// TODO: error recovery
		auto expr_list = parse_expr_list();
		if (!expr_list)
		{
			return expr_list.takeError();
		}

		arguments = std::move(*expr_list);
	}

	if (auto err = expect(lexeme_type::symb_close_parenthesis, true))
	{
		return std::move(err);
	}
	
	return std::make_unique<ir::ast::expression::call>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(func), std::move(arguments));
}

llvm::Expected<std::unique_ptr<ir::ast::expression::expression>> parser::parser::parse_expr()
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
			auto prefix_expr = parse_prefix_expr();
			if (!prefix_expr)
			{
				return prefix_expr.takeError();
			}

			auto expr = std::move(*prefix_expr);
			while (lexer.current_lexeme().pos.line == start.line)
			{
				switch (lexer.current_lexeme().type)
				{
					case lexeme_type::symb_open_parenthesis:
					{
						auto call_args = parse_call_expr_args(std::move(expr));
						if (!call_args)
						{
							return call_args.takeError();
						}
						expr = std::move(*call_args);
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
			return llvm::make_error<error_info>(filename, start, error_message.str());
		}
	}
}

llvm::Expected<std::unique_ptr<ir::ast::expression::expression>> parser::parser::parse_prefix_expr()
{
	const auto start = lexer.current_lexeme().pos;

	switch (lexer.current_lexeme().type)
	{
		case lexeme_type::symb_open_parenthesis:
		{
			lexer.next_lexeme();
			auto expr = parse_expr();
			if (!expr)
			{
				return expr.takeError();
			}

			if (auto err = expect(lexeme_type::symb_close_parenthesis, true))
			{
				return std::move(err);
			}
			return std::move(*expr);
		}
		case lexeme_type::identifier:
		{
			auto identifier_name = std::string{ lexer.current_lexeme().value };
			lexer.next_lexeme();
			auto unresolved_var = std::make_unique<ir::ast::expression::unresolved_variable>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(identifier_name));
			return std::make_unique<ir::ast::expression::variable>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(unresolved_var));
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '(' or identifier, got " << lexer.current_lexeme().to_string();
			return llvm::make_error<error_info>(filename, lexer.current_lexeme().pos, error_message.str());
		}
	}
}

llvm::Expected<std::unique_ptr<ir::ast::statement::extern_definition>> parser::parser::parse_extern_stat()
{
	position start = lexer.current_lexeme().pos;
	lexer.next_lexeme();
	
	if (auto err = expect(lexeme_type::kw_fn, true))
	{
		return std::move(err);
	}

	if (auto err = expect(lexeme_type::identifier))
	{
		return std::move(err);
	}

	// store function name
	const auto function_name = lexer.current_lexeme().value;
	lexer.next_lexeme();

	if (auto err = expect(lexeme_type::symb_open_parenthesis, true))
	{
		return std::move(err);
	}

	auto arg_list = parse_var_list();
	if (!arg_list)
	{
		return arg_list.takeError();
	}

	if (auto err = expect(lexeme_type::symb_close_parenthesis, true))
	{
		return std::move(err);
	}

	ir::ast::type return_type {};
	if (lexer.current_lexeme().type == lexeme_type::symb_arrow) // return type
	{
		lexer.next_lexeme();

		auto type = parse_type();
		if (!type)
		{
			return type.takeError();
		}
		return_type = std::move(*type);
	}
	else
	{
		return_type.name = "void";
		return_type.is_optional = false;
	}

	std::unordered_set<std::string> attributes;
	while (lexer.current_lexeme().type == lexeme_type::attribute)
	{
		attributes.insert(std::string{ lexer.current_lexeme().value });
		lexer.next_lexeme();
	}

	return std::make_unique<ir::ast::statement::extern_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::string{ function_name }, std::move(*arg_list),
		std::move(return_type), std::move(attributes));
}

llvm::Expected<std::unique_ptr<ir::ast::statement::function_definition>> parser::parser::parse_function_definition_stat()
{
	position start = lexer.current_lexeme().pos;
	lexer.next_lexeme();
	if (auto err = expect(lexeme_type::identifier))
	{
		return std::move(err);
	}

	// store function name
	const auto function_name = lexer.current_lexeme().value;
	lexer.next_lexeme();

	if (auto err = expect(lexeme_type::symb_open_parenthesis, true))
	{
		return std::move(err);
	}

	auto arg_list = parse_var_list();
	if (!arg_list)
	{
		return arg_list.takeError();
	}

	if (auto err = expect(lexeme_type::symb_close_parenthesis, true))
	{
		return std::move(err);
	}

	ir::ast::type return_type {};
	if (lexer.current_lexeme().type == lexeme_type::symb_arrow) // return type
	{
		lexer.next_lexeme();

		auto type = parse_type();
		if (!type)
		{
			return type.takeError();
		}
		return_type = std::move(*type);
	}
	else
	{
		return_type.name = "void";
		return_type.is_optional = false;
	}

	std::unordered_set<std::string> attributes;
	while (lexer.current_lexeme().type == lexeme_type::attribute)
	{
		attributes.insert(std::string{ lexer.current_lexeme().value });
		lexer.next_lexeme();
	}

	if (auto err = expect(lexeme_type::symb_open_brace))
	{
		return std::move(err);
	}

	auto block = parse_block_stat();
	if (!block)
	{
		return block.takeError();
	}

	return std::make_unique<ir::ast::statement::function_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::string{ function_name }, std::move(*arg_list),
		std::move(return_type), std::move(attributes), std::move(*block));
}

llvm::Expected<std::unique_ptr<ir::ast::statement::type_definition>> parser::parser::parse_type_definition_stat()
{
	lexer.next_lexeme();
	if (auto err = expect(lexeme_type::identifier))
	{
		return std::move(err);
	}
	const auto start = lexer.current_lexeme().pos;
	const auto type_name = std::string{ lexer.current_lexeme().value };

	lexer.next_lexeme();

	switch (lexer.current_lexeme().type)
	{
		case lexeme_type::symb_equals: // type <name> = <existing type>
		{
			lexer.next_lexeme();

			auto target_type_desc = parse_type();
			if (!target_type_desc)
			{
				return target_type_desc.takeError();
			}

			return std::make_unique<ir::ast::statement::alias_type_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos },
				type_name, std::move(*target_type_desc));
		}
		case lexeme_type::symb_open_brace: // type <name> { <type block> }
		{
			lexer.next_lexeme();

			std::vector<ir::ast::var> fields;
			std::vector<std::unique_ptr<ir::ast::statement::restricted_statement>> body;
			while (lexer.current_lexeme().type != lexeme_type::symb_close_brace)
			{
				if (lexer.current_lexeme().type == lexeme_type::identifier)
				{
					const auto field_name = std::string{ lexer.current_lexeme().value };
					lexer.next_lexeme();

					if (auto err = expect(lexeme_type::symb_colon, true))
					{
						return std::move(err);
					}

					auto type = parse_type();
					if (!type)
					{
						return type.takeError();
					}
		
					fields.emplace_back(std::move(*type), field_name);
				}
				else
				{
					auto restricted_stat = parse_restricted_stat();
					if (!restricted_stat)
					{
						return restricted_stat.takeError();
					}

					body.push_back(std::move(*restricted_stat));
				}
			}

			if (auto err = expect(lexeme_type::symb_close_brace, true))
			{
				return std::move(err);
			}

			auto body_stat = std::make_unique<ir::ast::statement::restricted_block>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(body));

			return std::make_unique<ir::ast::statement::class_type_definition>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(fields), std::move(body_stat));
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '=' or '{', got " << lexer.current_lexeme().to_string();
			return llvm::make_error<error_info>(filename, lexer.current_lexeme().pos, error_message.str());
		}
	}
}

llvm::Expected<std::unique_ptr<ir::ast::statement::ret>> parser::parser::parse_return_stat()
{
	const auto start = lexer.current_lexeme().pos;
	lexer.next_lexeme();

	std::unique_ptr<ir::ast::expression::expression> expr = nullptr;
	if (lexer.current_lexeme().type != lexeme_type::symb_close_brace
		&& lexer.current_lexeme().pos.line == start.line)
	{
		auto expr_ = parse_expr();
		if (!expr_)
		{
			return expr_.takeError();
		}
		expr = std::move(*expr_);
	}
	return std::make_unique<ir::ast::statement::ret>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(expr));
}

llvm::Expected<std::unique_ptr<ir::ast::statement::block>> parser::parser::parse_block_stat()
{
	position start = lexer.current_lexeme().pos;
	lexer.next_lexeme(); // {

	llvm::Error err = llvm::Error::success();

	std::vector<std::unique_ptr<ir::ast::statement::statement>> body;
	while (true)
	{
		position stat_start = lexer.current_lexeme().pos;
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
				auto return_stat = parse_return_stat();
				if (!return_stat)
				{
					return return_stat.takeError();
				}

				body.push_back(std::move(*return_stat));
				break;
			}
			default:
			{
				auto expr = parse_expr();
				if (!expr)
				{
					err = llvm::joinErrors(std::move(err), expr.takeError());

					while (lexer.current_lexeme().pos.line == stat_start.line
							&& lexer.current_lexeme().type != lexeme_type::eof)
					{
						lexer.next_lexeme();
					}

					continue;
				}

				ir::ast::expression::variable* var_expr;
				if (lexer.current_lexeme().type == lexeme_type::symb_colon
					&& (var_expr = dynamic_cast<ir::ast::expression::variable*>(expr.get().get()))) // var_def
				{
					lexer.next_lexeme();
					auto type = parse_type();
					if (!type)
					{
						err = llvm::joinErrors(std::move(err), type.takeError());

						while (lexer.current_lexeme().pos.line == stat_start.line
								&& lexer.current_lexeme().type != lexeme_type::eof)
						{
							lexer.next_lexeme();
						}
						continue;
					}
					
					auto expect_err = expect(lexeme_type::symb_declare, true);
					if (expect_err)
					{
						err = llvm::joinErrors(std::move(err), std::move(expect_err));
						while (lexer.current_lexeme().pos.line == stat_start.line
								&& lexer.current_lexeme().type != lexeme_type::eof)
						{
							lexer.next_lexeme();
						}
						continue;
					}
					
					auto value_expr = parse_expr();
					if (!value_expr)
					{
						err = llvm::joinErrors(std::move(err), value_expr.takeError());;
						while (lexer.current_lexeme().pos.line == stat_start.line
								&& lexer.current_lexeme().type != lexeme_type::eof)
						{
							lexer.next_lexeme();
						}
						continue;
					}

					body.push_back(std::make_unique<ir::ast::statement::variable_declaration>(
						ir::ast::position_range{ stat_start, lexer.current_lexeme().pos },
						ir::ast::var{ *type, reinterpret_cast<ir::ast::expression::unresolved_variable*>(var_expr->var.get())->name },
						std::move(*value_expr)));
					
					continue;
				}

				if (lexer.current_lexeme().type == lexeme_type::symb_equals) // expresison perhaps?
				{
					continue;
				}

				body.push_back(std::make_unique<ir::ast::statement::expression_statement>(
					ir::ast::position_range{ stat_start, lexer.current_lexeme().pos }, std::move(*expr)));
			}
		}
	}

	if (err)
	{
		return std::move(err);
	}

	return std::make_unique<ir::ast::statement::block>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(body));
}

llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_statement>> parser::parser::parse_restricted_stat()
{
	const auto& lexeme = lexer.current_lexeme();

	switch (lexeme.type)
	{
		case lexeme_type::kw_fn:
		{
			return parse_function_definition_stat();
		}
		case lexeme_type::kw_extern:
		{
			return parse_extern_stat();
		}
		case lexeme_type::kw_type:
		{
			return parse_type_definition_stat();
		}
		default:
		{
			std::stringstream error_message;
			error_message << "unexpected identifier " << lexeme.to_string() << " in restricted namespace, expected a type or function definition";
			return llvm::make_error<error_info>(filename, lexer.current_lexeme().pos, error_message.str());
		}
	}
}

llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_block>> parser::parser::parse_block_restricted_stat()
{
	const auto start = lexer.current_lexeme().pos;
	
	std::vector<std::unique_ptr<ir::ast::statement::restricted_statement>> body;
	while (true)
	{
		const auto& lexeme = lexer.current_lexeme();

		if (lexeme.type == lexeme_type::eof
			|| lexeme.type == lexeme_type::symb_close_brace)
		{
			break;
		}

		auto restricted_stat = parse_restricted_stat();
		if (!restricted_stat)
		{
			return restricted_stat.takeError();
		}

		body.push_back(std::move(*restricted_stat));
	}

	return std::make_unique<ir::ast::statement::restricted_block>(ir::ast::position_range{ start, lexer.current_lexeme().pos }, std::move(body));
}

llvm::Expected<std::unique_ptr<ir::ast::statement::restricted_block>> parser::parser::parse()
{
	lexer.next_lexeme();

	auto root = parse_block_restricted_stat();
	if (!root)
	{
		return root.takeError();
	}

	if (auto err = expect(lexeme_type::eof))
	{
		return std::move(err);
	}

	pass::invoke_all(root->get());

	return root;
}
