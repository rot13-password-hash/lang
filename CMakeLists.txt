cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(lang)

find_package(LLVM CONFIG REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(lang
    src/main.cpp
    src/compiler/lexer/lexer.cpp
    src/compiler/parser/parser.cpp
    src/compiler/code_gen/code_gen.cpp
    src/compiler/parser/passes/pass.cpp
    src/compiler/parser/passes/type_analyzer.cpp 
    "src/compiler/ir/ast/ast.cpp"
    "src/compiler/ir/cfg/cfg.cpp"
    src/debug/graphviz.cpp
    "src/compiler/ir/cfg/cfg_builder.cpp")

add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader x86codegen x86asmparser x86desc x86info x86utils)

# Link against LLVM libraries
target_link_libraries(lang ${llvm_libs})
# target_compile_options(lang PUBLIC -fno-rtti)