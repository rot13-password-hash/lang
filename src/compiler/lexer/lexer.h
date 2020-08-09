#include "lexeme.h"

#include <string_view>

namespace lang::compiler::lexer
{
    class lexer
    {
        lexeme current_lexeme;
    public:
        lexeme& next();
        lexeme& current();

        explicit lexer(std::string_view source) :
            source(source) {}
    };
}