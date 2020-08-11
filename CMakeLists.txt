cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(lang)

add_executable(lang
    src/main.cpp
    src/compiler/lexer/lexer.cpp
    src/compiler/parser/parser.cpp
    src/compiler/code_gen/code_gen.cpp)