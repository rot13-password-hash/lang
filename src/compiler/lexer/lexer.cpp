#include "lexer.h"

using namespace lang::compiler::lexer;

lexeme& lexer::next()
{
    return current_lexeme;
}

lexeme& lexer::current()
{
    return current_lexeme;
}