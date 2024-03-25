#ifndef LUA_PARSER_STATEMENT_STRUCTS_H
#define LUA_PARSER_STATEMENT_STRUCTS_H

#include "position.h"

typedef enum {
    STATEMENT_NONE = 0,
    Assignment
} StatementType;

typedef struct {
    Position position;
    StatementType type;
    void *value;
} Statement;

#endif //LUA_PARSER_STATEMENT_STRUCTS_H
