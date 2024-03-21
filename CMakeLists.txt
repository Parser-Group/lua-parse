cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 23)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

project(LuaParser C)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_executable(LuaParser
        src/main.c
        src/token.h
        src/Position.h
        src/lexer/lexer.h
        src/lexer/char_position.h
        src/utils.h
        src/token.c
        src/lexer/lexer.c
)
