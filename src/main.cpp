#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
#include "compiler/utils/exception.h"

#include <iostream>
#include <memory>

using namespace lang::compiler;

int main() {
    std::string source = R"s(
type a = i32
type b = i64
type c = i128
)s";

    /*
    lexer::lexer lexer{ source };

    while (true)
    {
        std::cout << lexer.next().to_string() << '\n';
        if (lexer.current().type == lexer::lexeme::lexeme_type::eof)
        {
            break;
        }
    }*/

	try
	{
		parser::parser parser{ source };
		parser.parse();
	}
	catch (const exception& ex)
	{
		std::cout << ex.pos.line << ':' << ex.pos.col << ": " << ex.what() << std::endl;
	}
}