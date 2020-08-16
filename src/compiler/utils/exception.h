#pragma once

#include <stdexcept>
#include <string>

#include "position.h"

namespace lang::compiler
{
	struct exception : public std::runtime_error
	{
		position pos;

		explicit exception(position pos, const std::string& msg) : std::runtime_error(msg.c_str()),
			pos(pos)
		{}
	};
}