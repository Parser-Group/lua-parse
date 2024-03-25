<<<<<<< HEAD
#ifndef LUA_PARSER_COMMON_H
#define LUA_PARSER_COMMON_H
=======
#ifndef LUAPARSER_UTILS_H
#define LUAPARSER_PARSER_UTILS_H
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define UNIMPLEMENTED(...)                                                      \
    do {                                                                        \
        printf("%s:%d: UNIMPLEMENTED: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                                \
    } while(0)

#define UNREACHABLE(...)                                                      \
    do {                                                                      \
        printf("%s:%d: UNREACHABLE: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                              \
    } while(0)

#define UNUSED(x) (void)(x)

#endif //LUA_PARSER_COMMON_H
