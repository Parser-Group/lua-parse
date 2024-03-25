#ifndef LUAPARSER_UTILS_H
#define LUAPARSER_PARSER_UTILS_H

#include "stdio.h"
#include "stdlib.h"

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

#endif //LUAPARSER_UTILS_H
