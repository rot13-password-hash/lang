#include "compiler/lexer/lexer.h"

#include <iostream>
#include <memory>

int main() {
    std::string source = R"s(
    print'hi'
    )s";

    lang::compiler::lexer::lexer lexer{ source };
}