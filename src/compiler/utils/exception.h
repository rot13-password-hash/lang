#pragma once

#include <stdexcept>
#include <string>

#include "position.h"

namespace lang::compiler
{
	struct exception : public std::exception
	{
		position pos;

		explicit exception(position pos, const std::string& msg) : std::exception(msg.c_str()),
			pos(pos)
		{}
	};
}