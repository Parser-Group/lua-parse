#ifndef LUA_PARSER_COMMON_H
#define LUA_PARSER_COMMON_H

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define UNIMPLEMENTED(...)                                                      \
    printf("%s:%d: UNIMPLEMENTED: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
    exit(1);

#define UNFINISHED(...)                                                     \
    printf("%s:%d: UNFINISHED: %s \n", __FILE__, __LINE__, __VA_ARGS__);

#define UNREACHABLE(...)                                                      \
    do {                                                                      \
        printf("%s:%d: UNREACHABLE: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                              \
    } while(0)

#define SAFE_MALLOC(type, ptr) \
    type *ptr = malloc(sizeof(type)); \
    if ((ptr) == NULL) { \
        printf("%s:%d: Memory Allocation failed\n", __FILE__, __LINE__); \
        exit(1); \
    }

#define SAFE_MALLOC_TO(ptr, size) \
    ptr = malloc(size); \
    if ((ptr) == NULL) { \
        do { \
            printf("%s:%d: Memory Allocation failed\n", __FILE__, __LINE__); \
            exit(1); \
        } while(0); \
    }

#endif //LUA_PARSER_COMMON_H
