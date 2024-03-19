#ifndef LUATRANSPILER_LEXER_H
#define LUATRANSPILER_LEXER_H
#include "token.h"

typedef struct {
    const char *content;
    uSize content_len;
    uSize cursor;
    uSize line;
    uSize begin_of_line;
} Lexer;

Lexer  lexer_new(const char *content, uSize content_len);
Token  lexer_next(Lexer *l);

#endif //LUATRANSPILER_LEXER_H
