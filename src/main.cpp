#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
#include "compiler/utils/exception.h"

#include "debug/graphviz.h"

#include <iostream>
#include <memory>

using namespace lang::compiler;

int main() {
    std::string source = R"s(
fn main() -> void @constructor
{
    print("hello world!")
}=
)s";

	try
	{
		parser::parser parser{ source };
		auto root = parser.parse();

        graphvizitor gviz;
        root->visit(&gviz);
        
        std::cout << gviz.string();
	}
	catch (const exception& ex)
	{
		std::cout << ex.pos.line << ':' << ex.pos.col << ": " << ex.what() << std::endl;
	}
}