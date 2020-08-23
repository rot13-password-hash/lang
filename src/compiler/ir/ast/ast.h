#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <unordered_set>


#include "../../utils/position.h"
#include "types.h"

namespace seam::compiler::ir::ast
{
	struct position_range
	{
		position start, end;
	};


	struct visitor;

	struct node
	{
		virtual void visit(visitor* vst) = 0;

		position_range range;

		virtual ~node() {}
	protected:
		node(position_range range) :
			range(range) {}
	};
	
	struct type
	{
		std::string name;
		bool is_optional;

		type() = default;

		type(std::string name, bool is_optional) :
			name(std::move(name)), is_optional(is_optional) {}

		type(type&& other) noexcept :
			name(std::move(other.name)),
			is_optional(other.is_optional) {}

		type(type& other) :
			name(other.name),
			is_optional(other.is_optional) {}

		type& operator=(type&& other) noexcept
		{
			name = std::move(other.name);
			is_optional = other.is_optional;

			return *this;
		}
	};
	
	using type_reference = std::variant<type, types::type_reference>;

	struct number
	{
		std::string value;

		number(std::string value) :
			value(std::move(value)) {}
	};

    struct var
    {
		type_reference type_;
        std::string name;

		var(type type_, std::string name) :
			type_(std::move(type_)), name(std::move(name)) {}
    };

	namespace expression
	{
		struct expression : node
		{
			expression(position_range range) :
				node(range) {}

			virtual ~expression() {}
		};

		template <typename T>
		struct literal : expression
		{
			T val;

			literal(position_range range, T val) :
				expression(range), val(std::move(val)) {}

			void visit(visitor* vst)
			{
				vst->visit(this);
			}
		};

		struct call : expression
		{
			std::unique_ptr<expression> func;
			std::vector<std::unique_ptr<expression>> arguments;

			call(position_range range, std::unique_ptr<expression> func, std::vector<std::unique_ptr<expression>> arguments) :
				expression(range), func(std::move(func)), arguments(std::move(arguments)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		struct unresolved_variable : expression
		{
			std::string name;

			unresolved_variable(position_range range, std::string name) :
				expression(range), name(std::move(name)) {}

			void visit(visitor* vst);
		};

		struct local_variable : expression
		{

		};

		struct module_variable : expression
		{

		};
	}

	namespace statement
	{
		struct statement : node
		{
			statement(position_range range) :
				node(range) {}

			virtual ~statement() {};
		};

		struct expression_statement : statement
		{
			std::unique_ptr<expression::expression> expr;
			expression_statement(position_range range, std::unique_ptr<expression::expression> expr) :
				statement(range), expr(std::move(expr)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		// only in top level scope and type definitions
		struct restricted_statement : node
		{
			restricted_statement(position_range range) :
				node(range) {}

			virtual ~restricted_statement() {};
		};

		struct block : statement
		{
			std::vector<std::unique_ptr<statement>> body;

			block(position_range range, std::vector<std::unique_ptr<statement>> body) :
				statement(range), body(std::move(body)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		struct function_definition : restricted_statement
		{
			std::string name;
			std::vector<var> arguments;
			type_reference return_type;
			std::unordered_set<std::string> attributes;

			std::unique_ptr<block> body_stat;

			function_definition(position_range range, std::string name, std::vector<var> arguments, type return_type,
				std::unordered_set<std::string> attributes, std::unique_ptr<block> body_stat) :
				restricted_statement(range),
				name(std::move(name)),
				arguments(std::move(arguments)),
				return_type(std::move(return_type)),
				attributes(std::move(attributes)),
				body_stat(std::move(body_stat))
			{}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		struct type_definition : restricted_statement
		{
			using restricted_statement::restricted_statement;

			virtual ~type_definition() {}
		};

		struct alias_type_definition : type_definition
		{
			std::string alias_name;
			type_reference target_type;

			alias_type_definition(position_range range, std::string alias_name, type target_type) :
				type_definition(range), alias_name(std::move(alias_name)), target_type(std::move(target_type)) {}

			void visit(visitor* vst);
		};

		struct restricted_block : statement
		{
			std::vector<std::unique_ptr<restricted_statement>> body;

			restricted_block(position_range range, std::vector<std::unique_ptr<restricted_statement>> body) :
				statement(range), body(std::move(body)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		struct class_type_definition : type_definition
		{
			std::string name;

			std::vector<var> fields;
			std::unique_ptr<restricted_block> body;

			class_type_definition(position_range range, std::vector<var> fields, std::unique_ptr<restricted_block> body) :
				type_definition(range), fields(std::move(fields)), body(std::move(body)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};

		struct ret : statement
		{
			std::unique_ptr<expression::expression> value; // can be nullptr

			ret(position_range range, std::unique_ptr<expression::expression> value) :
				statement(range), value(std::move(value)) {}

			void visit_children(visitor* vst);
			void visit(visitor* vst);
		};
	}

	struct module
	{
		module(std::string relative_path, std::unique_ptr<statement::restricted_block> body, bool is_root)
			: relative_path(std::move(relative_path)), body(std::move(body)), is_root(is_root) {}

		std::string relative_path;
		std::unique_ptr<statement::restricted_block> body;
		bool is_root; // is this module the file being compiled?
	};

#define BASE_VISITOR(c) virtual bool visit(c* a) { return true; }
#define VISITOR(b, c) virtual bool visit(c* a) { return visit(static_cast<b*>(a)); }

	struct visitor
	{
		virtual ~visitor() {}

		BASE_VISITOR(node);

		VISITOR(node, expression::expression);
		VISITOR(node, statement::statement);
		VISITOR(node, statement::restricted_statement);

		VISITOR(expression::expression, expression::literal<std::string>);
		VISITOR(expression::expression, expression::literal<number>);
		VISITOR(expression::expression, expression::literal<std::int8_t>);
		VISITOR(expression::expression, expression::literal<std::int16_t>);
		VISITOR(expression::expression, expression::literal<std::int32_t>);
		VISITOR(expression::expression, expression::literal<std::int64_t>);
		VISITOR(expression::expression, expression::literal<std::uint8_t>);
		VISITOR(expression::expression, expression::literal<std::uint16_t>);
		VISITOR(expression::expression, expression::literal<std::uint32_t>);
		VISITOR(expression::expression, expression::literal<std::uint64_t>);
		VISITOR(expression::expression, expression::literal<float>);
		VISITOR(expression::expression, expression::literal<double>);
		VISITOR(expression::expression, expression::literal<bool>);

		VISITOR(expression::expression, expression::call);
		VISITOR(expression::expression, expression::unresolved_variable);

		VISITOR(statement::restricted_statement, statement::type_definition);
		VISITOR(statement::restricted_statement, statement::function_definition)

		VISITOR(statement::type_definition, statement::alias_type_definition);
		VISITOR(statement::type_definition, statement::class_type_definition);

		VISITOR(statement::statement, statement::block);
		VISITOR(statement::statement, statement::restricted_block);
		VISITOR(statement::statement, statement::ret);
	};

#undef BASE_VISITOR
#undef VISITOR
}