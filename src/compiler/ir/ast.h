#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <optional>

#include "../utils/position.h"

namespace lang::compiler::ir::ast
{
	struct position_range
	{
		position start, end;
	};

	struct node
	{
		position_range range;

	protected:
		node(position_range range) :
			range(range) {}
	};

	struct type
	{
		std::string name;

		type() = default;

		type(std::string name) :
			name(std::move(name)) {}

		type(type&& other) :
			name(std::move(other.name)) {}
	};

    struct var
    {
		type type;
        std::string name;

		//var(type type, std::string name) :
			//type(type), name(std::move(name)) {}
    };

	namespace expression
	{
		struct expression : node
		{
			expression(position_range range) :
				node(range) {}
		};
	}

	namespace statement
	{
		struct statement : node
		{
			statement(position_range range) :
				node(range) {}
		};

		// only in top level scope and type definitions
		struct restricted_statement : node
		{
			restricted_statement(position_range range) :
				node(range) {}
		};

		struct block : statement
		{
			std::vector<std::unique_ptr<statement>> body;

			block(position_range range, std::vector<std::unique_ptr<statement>> body) :
				statement(range), body(std::move(body)) {}
		};

		struct function_definition : restricted_statement
		{
			std::string name;
			std::vector<var> arguments;
			type return_type;

			std::unique_ptr<block> body_stat;

			function_definition(position_range range, std::string name, std::vector<var> arguments, type return_type, std::unique_ptr<block> body_stat) :
				restricted_statement(range),
				name(std::move(name)),
				arguments(std::move(arguments)),
				return_type(std::move(return_type))
			{}
		};

		struct define_type_alias : restricted_statement
		{
			std::string alias_name;
			type target_type;

			define_type_alias(position_range range) :
				restricted_statement(range) {}
		};

		struct define_type_definition : restricted_statement
		{
			std::string name;

			struct field
			{
				var variable;
				std::unique_ptr<expression::expression> value = nullptr;
			};

			std::vector<field> fields;
			std::vector<std::unique_ptr<restricted_statement>> body;

			define_type_definition(position_range range, std::vector<std::unique_ptr<restricted_statement>> body) :
				restricted_statement(range), body(std::move(body)) {}
		};

		struct top_level_block : statement
		{
			std::vector<std::unique_ptr<restricted_statement>> body;

			top_level_block(position_range range, std::vector<std::unique_ptr<restricted_statement>> body) :
				statement(range), body(std::move(body)) {}
		};
	}
}