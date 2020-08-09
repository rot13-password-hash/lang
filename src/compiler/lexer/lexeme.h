namespace lang::compiler::lexer
{
    struct lexeme
    {
        enum class lexeme_type
        {
            eof
        };

        lexeme_type type = lexeme_type::eof;
    }
}