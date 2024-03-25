#ifndef LUAPARSER_PARSER_STRUCTS_H
#define LUAPARSER_PARSER_STRUCTS_H

#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;
    Token cur_token;
} Parser;

#endif //LUAPARSER_PARSER_STRUCTS_H
