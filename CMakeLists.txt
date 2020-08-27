cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_CXX_STANDARD 17)

project(seam)

add_subdirectory(runtime)

find_package(LLVM CONFIG REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(seam
    src/main.cpp
    src/compiler/lexer/lexer.cpp
    src/compiler/parser/parser.cpp
    src/compiler/code_gen/code_gen.cpp
    src/compiler/parser/passes/pass.cpp
    src/compiler/parser/passes/type_analyzer.cpp 
    src/compiler/ir/ast/ast.cpp
    src/compiler/ir/cfg/cfg.cpp
    src/debug/graphviz.cpp
    src/compiler/ir/cfg/cfg_builder.cpp
    src/compiler/parser/passes/symbol_collector.cpp
    src/compiler/compiler.cpp
    src/compiler/utils/error.cpp 
    src/compiler/parser/passes/variable_resolver.cpp)

add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD})

# Link against LLVM libraries
target_link_libraries(seam ${llvm_libs})

set_property(TARGET seam PROPERTY
             VS_DEBUGGER_COMMAND_ARGUMENTS "--help")

set_property(TARGET seam PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")