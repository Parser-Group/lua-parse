cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 23)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

project(Lua_Parser)
set(CMAKE_PROJECT_NAME Lua_Parser)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(Lua_Parser ${HEADERS} ${SOURCES})
