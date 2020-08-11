#include "parser.h"

using namespace lang::compiler;

ir::ast::position parser::parser::current_position()
{
    return { lexer.current().line, lexer.current().col };
}

std::unique_ptr<ir::ast::statement::block> parser::parser::parse()
{
    lexer.next();

    ir::ast::position start = current_position();
    return std::make_unique<ir::ast::statement::block>(ir::ast::position_range{ start, current_position() });
}