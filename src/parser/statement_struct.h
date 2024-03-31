#ifndef LUA_PARSER_STATEMENT_STRUCT_H
#define LUA_PARSER_STATEMENT_STRUCT_H

#include "position.h"

typedef enum {
    STATEMENT_NONE = 0,
    STATEMENT_END,

    STATEMENT_EXPRESSION,
    STATEMENT_LOCAL,
    STATEMENT_FUNCTION_DECLARATION,
    STATEMENT_VARIABLE_DECLARATION,
    STATEMENT_ASSIGNMENT,

    STATEMENT_DO,
    STATEMENT_RETURN,

    STATEMENT_IF,
    
    STATEMENT_FOR_NUMERIC,
    STATEMENT_FOR_GENERIC,
    STATEMENT_REPEAT,
    STATEMENT_WHILE,
    STATEMENT_BREAK,
    
    STATEMENT_GOTO_POINT,
    STATEMENT_GOTO,
    
} StatementType;

typedef struct {
    Position position;
    StatementType type;
    void *value;
} Statement;

typedef struct StatementNode {
    struct StatementNode *next;
    Statement value;
} StatementNode;

#endif //LUA_PARSER_STATEMENT_STRUCT_H
