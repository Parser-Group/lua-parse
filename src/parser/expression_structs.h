#ifndef LUAPARSER_EXPRESSION_STRUCTS_H
#define LUAPARSER_EXPRESSION_STRUCTS_H

#include "position.h"

typedef enum {
    EXPRESSION_NONE = 0,
} ExpressionType;

typedef struct {
    Position position;
    ExpressionType  type;
    void *value;
} Expression;

#endif //LUAPARSER_EXPRESSION_STRUCTS_H
