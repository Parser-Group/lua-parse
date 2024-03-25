#ifndef LUAPARSER_STATEMENT_STRUCTS_H
#define LUAPARSER_STATEMENT_STRUCTS_H

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

#endif //LUAPARSER_STATEMENT_STRUCTS_H
