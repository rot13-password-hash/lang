#pragma once

#include <stdexcept>
#include <string>

#include "position.h"

namespace seam::compiler
{
	struct exception : std::runtime_error
	{
		position pos;

		explicit exception(position pos, const std::string& msg) : std::runtime_error(msg.c_str()),
			pos(pos)
		{}
	};
}