#include "lexer.h"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <limits>

using namespace lang::compiler;

// reserved names must be lowercase
std::unordered_map<std::string_view, lexer::lexeme::lexeme_type> reserved_names =
{
    { "and", lexer::lexeme::lexeme_type::reserved_and },
    { "break", lexer::lexeme::lexeme_type::reserved_break },
    { "do", lexer::lexeme::lexeme_type::reserved_do },
    { "else", lexer::lexeme::lexeme_type::reserved_else },
    { "elseif", lexer::lexeme::lexeme_type::reserved_elseif },
    { "end", lexer::lexeme::lexeme_type::reserved_end },
    { "false", lexer::lexeme::lexeme_type::reserved_false },
    { "for", lexer::lexeme::lexeme_type::reserved_for },
    { "function", lexer::lexeme::lexeme_type::reserved_function },
    { "if", lexer::lexeme::lexeme_type::reserved_if },
    { "in", lexer::lexeme::lexeme_type::reserved_in },
    { "local", lexer::lexeme::lexeme_type::reserved_local },
    { "nil", lexer::lexeme::lexeme_type::reserved_nil },
    { "not", lexer::lexeme::lexeme_type::reserved_not },
    { "or", lexer::lexeme::lexeme_type::reserved_or },
    { "repeat", lexer::lexeme::lexeme_type::reserved_repeat },
    { "return", lexer::lexeme::lexeme_type::reserved_return },
    { "then", lexer::lexeme::lexeme_type::reserved_then },
    { "true", lexer::lexeme::lexeme_type::reserved_true },
    { "until", lexer::lexeme::lexeme_type::reserved_until },
    { "while", lexer::lexeme::lexeme_type::reserved_while },
};

inline bool is_newline(char c)
{
    return c == '\n';
}

inline bool is_whitespace(char c)
{
    return c == ' ' || (((unsigned)c - '\t') < 4) || c == '\r';
}

inline bool is_digit(char c)
{
    return ((unsigned)c - '0') < 10;
}

inline bool is_upper(char c)
{
    return static_cast<unsigned char>(c - 'A') < 26;
}

inline bool is_lower(char c)
{
    return static_cast<unsigned char>(c - 'a') < 26;
}

inline bool is_alpha(char c)
{
    return ((unsigned)c | 32) - 'a' < 26;
}

inline char unescape(char c)
{
    switch (c)
    {
    case 'a': return '\a';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    default: return c;
    }
}

void lexer::lexer::consume_newline()
{
    line++;
	line_offset = ++offset;
}

char lexer::lexer::peek(std::size_t off)
{
    return offset + off >= source.length() ? 0 : source[offset + off];
}

void lexer::lexer::consume_comment()
{
    throw std::runtime_error("not implemented");
}

void lexer::lexer::consume_whitespace()
{
    char c;
    while (is_whitespace(c = peek()))
    {
        if (is_newline(c))
        {
            consume_newline();
        }
        else
        {
            offset++;
        }
    }
}

void lexer::lexer::consume()
{
    if (is_newline(peek()))
    {
        consume_newline();
    }
    else
    {
        offset++;
    }
}

// returns true if all characters are uppercase
bool lexer::lexer::read_name(std::string_view& view)
{
    bool is_all_upper = true;

    std::size_t start_offset = offset;
    char c = peek();
    while (is_alpha(c) || is_digit(c) || c == '_')
    {
        if (is_lower(c))
        {
            is_all_upper = false;
        }
        c = source[++offset];
    }

    std::string_view temp{ &source[start_offset], offset - start_offset };
    view.swap(temp);

    return is_all_upper;
}

char lexer::lexer::read_escaped_char()
{
    switch (char c = peek())
    {
    case '\n':
        consume_newline();
        return c;
    case '\r': // TODO: test
        offset++;
        if (peek() == '\n')
        {
            consume_newline();
        }
        return '\n';
    case 0: // eof
        throw std::runtime_error("unfinished string");
    default:
    {
        if (is_digit(c))
        {
            int code = 0;
            int i = 0;

            do
            {
                code = 10 * code + (peek() - '0');
                offset++;
            } while (++i < 3 && is_digit(peek()));

            if (code > std::numeric_limits<std::uint8_t>::max())
                throw std::runtime_error("escape sequence too large");

            return (char)code;
        }

        char result = unescape(c);
        consume();

        return result;
    }
    }
}

std::string_view lexer::lexer::read_string(char delim)
{
    scratch.clear();

    char curr;
    while ((curr = peek()) != delim)
    {
        switch (curr)
        {
        case 0: // eof
        case '\r':
        case '\n':
            throw std::runtime_error("unfinished string");
        case '\\':
            offset++;
            scratch += read_escaped_char();
            continue;
        default:
            scratch += curr;
            offset++;
        }
    }

    // closing delimiter
    offset++;

    return { scratch };
}

const lexer::lexeme& lexer::lexer::next()
{
    while (true)
    {
        if (is_whitespace(peek()))
        {
            consume_whitespace();
        }

        if (peek() == '-' && peek(1) == '-')
        {
            consume_comment();
            continue;
        }

        break;
    }

    if (offset >= source.length())
    {
        current_lexeme.type = lexeme::lexeme_type::eof;
        return current_lexeme;
    }

    char first_char = peek();
    // can be a reserved name
    if (is_lower(first_char))
    {
        std::string_view name;
        bool name_is_upper = read_name(name);
        // can **still** be a reserved name
        if (!name_is_upper)
        {
            // TODO: use perfect hash
            const auto& it = reserved_names.find(name);
            if (it != reserved_names.cend())
            {
                current_lexeme.type = it->second;
                return current_lexeme;
            }
        }
        current_lexeme.type = lexeme::lexeme_type::name;
        current_lexeme.value = name;
    }
    else if (is_upper(first_char) || first_char == '_')
    {
        // TODO: make seperate read_name that doesnt return bool
        std::string_view name;
        read_name(name);

        current_lexeme.type = lexeme::lexeme_type::name;
        current_lexeme.value = name;
    }
    else
    {
        switch (first_char)
        {
        case '"':
        case '\'':
        {
            offset++;
            current_lexeme.type = lexeme::lexeme_type::string;
            current_lexeme.value = read_string(first_char);
            return current_lexeme;
        }
        case '(':
        {
            offset++;
            current_lexeme.type = lexeme::lexeme_type::open_parenthesis;
            return current_lexeme;
        }
        case ')':
        {
            offset++;
            current_lexeme.type = lexeme::lexeme_type::close_parenthesis;
            return current_lexeme;
        }
        case '[':
        {
            char next_char = peek(1);
            if (next_char == '=' || next_char == '[')
            {
                throw std::runtime_error("not implemented");
            }
            else
            {
                offset++;
                current_lexeme.type = lexeme::lexeme_type::open_bracket;
            }
        }
        default:
        {
            throw std::runtime_error("unexpected symbol");
        }
        }
    }
    
    return current_lexeme;
}

const lexer::lexeme& lexer::lexer::current()
{
    return current_lexeme;
}