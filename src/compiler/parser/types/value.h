#pragma once

#include "types.h"

#include <optional>
#include <vector>

namespace lang::compiler::parser
{
	template <typename T>
	struct optional_value
	{
		std::optional<typename T::value_type> val;
	};

	template <typename T>
	struct value
	{
		typename T::value_type val;
	};

	template <typename T>
	struct array_value
	{
		std::size_t count;
		std::vector<typename T::value_type> vals;
	};
}