#ifndef LUA_PARSER_PARSER_STRUCTS_H
#define LUA_PARSER_PARSER_STRUCTS_H

#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;
    Token cur_token;
} Parser;

#endif //LUA_PARSER_PARSER_STRUCTS_H
