#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <optional>
#include <vector>

#include "../utils/position.h"

namespace lang::compiler::ir::ast
{
	struct position_range
	{
		position start, end;
	};

	struct visitor;

	struct node
	{
		virtual visit(visitor* vst) = 0;

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

		template <typename literal_t>
		struct literal_expression : expression
		{
			literal_t val;

			literal_expression(position_range range, literal_t val) :
				expression(range), val(std::move(val)) {}
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

		struct type_definition : restricted_statement
		{
			using restricted_statement::restricted_statement;
		};

		struct alias_type_definition : type_definition
		{
			std::string alias_name;
			type target_type;

			alias_type_definition(position_range range, std::string alias_name, type target_type) :
				type_definition(range), alias_name(std::move(alias_name)), target_type(std::move(target_type)) {}
		};

		struct class_type_definition : type_definition
		{
			std::string name;

			struct field
			{
				var variable;
				std::unique_ptr<expression::expression> value = nullptr;
			};

			std::vector<field> fields;
			std::vector<std::unique_ptr<restricted_statement>> body;

			class_type_definition(position_range range, std::vector<std::unique_ptr<restricted_statement>> body) :
				type_definition(range), body(std::move(body)) {}
		};

		struct top_level_block : statement
		{
			std::vector<std::unique_ptr<restricted_statement>> body;

			top_level_block(position_range range, std::vector<std::unique_ptr<restricted_statement>> body) :
				statement(range), body(std::move(body)) {}
		};

		struct ret : statement
		{
			std::unique_ptr<expression::expression> value;

			ret(position_range range, std::unique_ptr<expression::expression> value) :
				statement(range), value(std::move(value)) {}
		};
	}

	struct visitor
	{
		bool visit()
	};
}