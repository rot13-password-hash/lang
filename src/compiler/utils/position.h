#pragma once

#include <stddef.h>

namespace lang::compiler
{
	struct position
	{
		std::size_t line, col;
	};
}