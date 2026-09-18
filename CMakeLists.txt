cmake_minimum_required(VERSION 3.16)
project(Marex VERSION 1.2.0 LANGUAGES CXX
        DESCRIPTION "A tiny interpreted, dynamically typed language")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Single source of truth for the version is project() above; the header
# is generated so the binary always reports the version it was built from.
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated/version.hpp
        @ONLY
)

add_executable(marex
        src/main.cpp
        src/cli.cpp
        src/runtime.cpp
        src/repl.cpp
        src/lexer.cpp
        src/parser.cpp
        src/interpreter.cpp
        src/values_types.cpp
        src/style.cpp
)

target_include_directories(marex PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/generated
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(marex PRIVATE -Wall -Wextra)
endif()

install(TARGETS marex RUNTIME DESTINATION bin)

enable_testing()
add_subdirectory(tests)
