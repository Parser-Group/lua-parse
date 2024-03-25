cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 23)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

project(LuaParser C)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_executable(LuaParser
        src/main.c
        src/token.h
        src/position.h
        src/lexer/lexer.h
        src/lexer/char_position.h
        src/utils.h
        src/token.c
        src/lexer/lexer.c
        src/parser/parser.c
        src/parser/parser.h
        src/parser/statement.h
        src/parser/parser_structs.h
        src/parser/expressions.c
        src/parser/expressions.h
        src/parser/statement.c
        src/parser/expression_structs.h
        src/parser/statement_structs.h
        src/parser/parser_utils.h
        src/parser/parser_utils.c
)
