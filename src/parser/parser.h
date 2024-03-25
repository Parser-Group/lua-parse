#ifndef LUA_PARSER_PARSER_H
#define LUA_PARSER_PARSER_H
#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;
} Parser;

Parser parser_new(Lexer *l);

#endif //LUA_PARSER_PARSER_H
