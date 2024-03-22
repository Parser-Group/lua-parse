#ifndef LUAPARSER_LEXER_H
#define LUAPARSER_LEXER_H
#include "token.h"

typedef struct {
    const char *content;
    size_t content_len;
    size_t cursor;
    size_t line;
    size_t begin_of_line;
} Lexer;

Lexer  lexer_new(const char *content, size_t content_len);
Token  lexer_next(Lexer *l);

#endif //LUAPARSER_LEXER_H
