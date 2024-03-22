#ifndef LUAPARSER_STATEMENT_H
#define LUAPARSER_STATEMENT_H

#include "Position.h"
#include "parser.h"

typedef enum {
    None = 0,
    Assignment
} StatementType;

typedef struct {
    Position position;
    StatementType type;
} Statement;

Statement statement_parse(Parser *p) {
    Token token = lexer_next(p->lexer);
    
    
}

#endif //LUAPARSER_STATEMENT_H
