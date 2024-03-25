#ifndef LUA_PARSER_EXPRESSION_STRUCTS_H
#define LUA_PARSER_EXPRESSION_STRUCTS_H

#include "position.h"

typedef enum {
    EXPRESSION_NONE = 0,

    EXPRESSION_VARIABLE,
    EXPRESSION_VARIABLE_INDEX,
    EXPRESSION_VARIABLE_NAME_INDEX,
    
    EXPRESSION_BINARY,
} ExpressionType;

typedef struct {
    Position position;
    ExpressionType  type;
    void *value;
} Expression;

//NOTE: modify numbers to be smaller for smaller memory consumption
// are the same numbers as TokenType for performance
typedef enum {
    BINARY_EXPRESSION_NONE = 0,
    
    BINARY_EXPRESSION_BIT_NOT = 10,
    BINARY_EXPRESSION_BIT_OR = 11,
    BINARY_EXPRESSION_BIT_AND = 12,
    BINARY_EXPRESSION_BIT_SHIFT_LEFT = 13,
    BINARY_EXPRESSION_BIT_SHIFT_RIGHT = 14,

    BINARY_EXPRESSION_EQUALS = 20,
    BINARY_EXPRESSION_NOT_EQUALS = 21,
    BINARY_EXPRESSION_GREATER_THAN = 22,
    BINARY_EXPRESSION_GREATER_THAN_OR_EQUAL = 23,
    BINARY_EXPRESSION_LESS_THAN = 24,
    BINARY_EXPRESSION_LESS_THAN_OR_EQUAL = 25,
    
    BINARY_EXPRESSION_STRING_CONCAT = 30,
    
    BINARY_EXPRESSION_PLUS = 41,
    BINARY_EXPRESSION_MINUS = 42,
    BINARY_EXPRESSION_MULTIPLY = 43,
    BINARY_EXPRESSION_DIVIDE = 44,
    BINARY_EXPRESSION_POWER = 45,
    BINARY_EXPRESSION_MODULO = 46,
} BinaryExpressionType;

typedef struct {
    Expression *parent;
    BinaryExpressionType type;
    Expression *left;
    Expression *right;
} BinaryExpression;

typedef struct {
    Expression *parent;
    const char *text;
    size_t text_len;
} VariableExpression;

typedef struct {
    Expression *parent;
    Expression *first;
    const char *index;
    size_t index_len;
} VariableNameIndex;

typedef struct {
    Expression *parent;
    Expression *first;
    Expression index;
} VariableIndex;

#endif //LUA_PARSER_EXPRESSION_STRUCTS_H
