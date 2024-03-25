#ifndef LUA_PARSER_STATEMENT_H
#define LUA_PARSER_STATEMENT_H

#include "parser.h"

typedef enum {
    STATEMENT_END = 0,

    // local
    STATEMENT_LOCAL_VARIABLE_DECLARATION,
    STATEMENT_LOCAL_FUNCTION_DECLARATION,
    
    } StatementType;

typedef struct {
    Position position;
    StatementType type;
    void *value;
} Statement;

Statement parser_statement(Parser *p);

#endif //LUA_PARSER_STATEMENT_H
