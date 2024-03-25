cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 23)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

project(Lua_Parser C)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_executable(Lua_Parser
        src/main.c
        src/token.h
        src/position.h
        src/lexer/lexer.h
        src/lexer/char_position.h
        src/common.h
        src/token.c
        src/lexer/lexer.c
        src/parser/parser.c
        src/parser/parser.h
        src/parser/statement.h
        src/parser/statement.c
        "src/parser/variable_declaration.h"
        src/position.c
)
