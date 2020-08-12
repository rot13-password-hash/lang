#pragma once

#include "types.h"

#include <optional>

namespace lang::compiler::parser
{
	template <typename T>
	struct nullable_value
	{
		std::optional<typename T::value_type> val;
	};

	template <typename T>
	struct value
	{
		typename T::value_type val;
	};
}