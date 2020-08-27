cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_CXX_STANDARD 17)

project(seam-runtime)

add_library(seam-runtime SHARED src/string.cpp)

set_property(TARGET seam-runtime PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")