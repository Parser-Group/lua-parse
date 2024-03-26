#ifndef LUA_PARSER_STATEMENT_STRUCT_H
#define LUA_PARSER_STATEMENT_STRUCT_H

#include "position.h"

typedef enum {
    STATEMENT_NONE = 0,

    STATEMENT_LOCAL,
    STATEMENT_FUNCTION,
    STATEMENT_VARIABLE_ASSIGNMENT,
} StatementType;

typedef struct {
    Position position;
    StatementType type;
    void *value;
} Statement;

typedef struct StatementNode {
    struct StatementNode *next;
    Statement *value;
} StatementNode;

#endif //LUA_PARSER_STATEMENT_STRUCT_H
