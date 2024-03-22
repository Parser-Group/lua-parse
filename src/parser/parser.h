#ifndef LUAPARSER_PARSER_H
#define LUAPARSER_PARSER_H
#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;
} Parser;

Parser parser_new(Lexer *l);

#endif //LUAPARSER_PARSER_H
