cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 23)

# Profiling
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(CMAKE_PROJECT_NAME Lua_Parser)
project(${CMAKE_PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})
