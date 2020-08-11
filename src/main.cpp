#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"

#include <iostream>
#include <memory>

using namespace lang::compiler;

int main() {
    std::string source = R"s(
    print'hi'
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

    parser::parser parser{ source };

    parser.parse();
}