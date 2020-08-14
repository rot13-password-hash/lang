#include <stack>

namespace lang::runtime
{
	class call_frame
	{

	};

	class state
	{
		std::stack<call_frame> call_stack;
	};
}