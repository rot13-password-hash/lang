#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <string>

namespace lang::compiler::ir::ast
{
	struct position
	{
		std::size_t line, col;
	};

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

	namespace statement
	{
		struct type
		{
			std::string name;

			type(std::string name) :
				name(std::move(name)) {}
		};

		struct block : node
		{
			block(position_range range) :
				node(range) {}
		};
	}

	namespace expression
	{

	}
}