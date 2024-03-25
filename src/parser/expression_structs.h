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
    
    BINARY_EXPRESSION_BIT_NOT = 50,
    BINARY_EXPRESSION_BIT_OR = 51,
    BINARY_EXPRESSION_BIT_AND = 52,
    BINARY_EXPRESSION_BIT_SHIFT_LEFT = 53,
    BINARY_EXPRESSION_BIT_SHIFT_RIGHT = 54,

    BINARY_EXPRESSION_EQUALS = 60,
    BINARY_EXPRESSION_NOT_EQUALS = 61,
    BINARY_EXPRESSION_GREATER_THAN = 62,
    BINARY_EXPRESSION_GREATER_THAN_OR_EQUAL = 63,
    BINARY_EXPRESSION_LESS_THAN = 64,
    BINARY_EXPRESSION_LESS_THAN_OR_EQUAL = 65,
    
    BINARY_EXPRESSION_STRING_CONCAT = 70,
    
    BINARY_EXPRESSION_PLUS = 101,
    BINARY_EXPRESSION_MINUS = 102,
    BINARY_EXPRESSION_MULTIPLY = 103,
    BINARY_EXPRESSION_DIVIDE = 104,
    BINARY_EXPRESSION_POWER = 105,
    BINARY_EXPRESSION_MODULO = 106,
    
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
