#pragma once

#include "../ir/ast.h"
#include "../lexer/lexer.h"

#include "types/types.h"
#include "types/value.h"

#include <string_view>
#include <memory>
#include <unordered_map>

namespace lang::compiler::parser
{
	class parser
	{
		lexer::lexer lexer;

		std::unordered_map<std::string, std::unique_ptr<type_desc>> type_map;

		ir::ast::position current_position();

	public:
		template <class T>
		void register_type(const std::string& name)
		{
			type_map[name] = std::make_unique<T>(name);
		}

		explicit parser(std::string_view source) :
			lexer(source)
		{
			register_type<i32_t>("i32");
			register_type<i64_t>("i64");
			register_type<u32_t>("u32");
			register_type<u64_t>("u64");
			register_type<bool_t>("bool");
			register_type<f32_t>("f32");
			register_type<f64_t>("f64");
			register_type<string_t>("string");
		}

		std::unique_ptr<ir::ast::statement::block> parse();
	};
}