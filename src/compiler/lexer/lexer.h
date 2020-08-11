#pragma once

#include "lexeme.h"

#include <string_view>
#include <cstddef>

namespace lang::compiler::lexer
{
    class lexer
    {
        std::string scratch;

        std::string_view source;
        std::size_t offset = 0;
        std::size_t line = 0;
        std::size_t line_offset = 0;

        lexeme current_lexeme;

        void consume_newline();
        char peek(std::size_t off = 0);
        void consume_comment();
        void consume_whitespace();
        void consume();

        // returns true if all characters are uppercase
        bool read_name(std::string_view& view);
        char read_escaped_char();
        std::string_view read_string(char delim);
    public:
        const lexeme& next();
        const lexeme& current();
        
        explicit lexer(const std::string_view& source) :
            source(source) {}
    };
}