#ifndef LUA_PARSER_EXPRESSION_STRUCTS_H
#define LUA_PARSER_EXPRESSION_STRUCTS_H

#include "position.h"
#include "symbol.h"
#include "statement_struct.h"

typedef enum {
    EXPRESSION_NONE = 0,

    EXPRESSION_PRIORITY,

    EXPRESSION_NOT,
    EXPRESSION_BIT_NOT,
    EXPRESSION_COUNT,

    EXPRESSION_LITERAL_NIL,
    EXPRESSION_LITERAL_BOOLEAN,
    EXPRESSION_LITERAL_NUMBER,
    EXPRESSION_LITERAL_STRING,

    EXPRESSION_VARIABLE,
    EXPRESSION_VARIABLE_INDEX,
    EXPRESSION_VARIABLE_NAME_INDEX,
    EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF,

    EXPRESSION_BINARY,
    EXPRESSION_FUNCTION,
    EXPRESSION_FUNCTION_CALL,

    EXPRESSION_TABLE,
} ExpressionType;

typedef struct {
    Position position;
    ExpressionType type;
    void *value;
} Expression;

typedef struct ExpressionNode {
    struct ExpressionNode *next;
    Expression *value;
} ExpressionNode;

typedef struct {
    Expression value;
} PriorityExpression;

typedef struct {
    Expression value;
} NotExpression;

typedef struct {
    Expression value;
} BitNotExpression;

typedef struct {
    bool value;
} LiteralBooleanExpression;

typedef struct {
    double value;
} LiteralNumberExpression;

typedef struct {
    const char *value;
    size_t value_len;
} LiteralStringExpression;

//NOTE: values are the same as TokenType for performance
typedef enum {
    BINARY_EXPRESSION_NONE = 0,

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

    BINARY_EXPRESSION_OR = 50,
    BINARY_EXPRESSION_AND = 51,
} BinaryExpressionType;

typedef struct {
    BinaryExpressionType type;
    Expression left;
    Expression right;
} BinaryExpression;

typedef struct {
    Symbol *symbol;
} VariableExpression;

typedef struct {
    Expression first;
    Symbol *index;
} VariableNameIndexExpression;

typedef struct {
    Expression first;
    Symbol *index;
} VariableNameIndexWithSelfExpression;

typedef struct {
    Expression first;
    Expression index;
} VariableIndexExpression;

typedef struct {
    Position position;
    Symbol *name;
} FunctionParameter;

typedef struct FunctionParameterNode {
    struct FunctionParameterNode *next;
    FunctionParameter *value;
} FunctionParameterNode;

typedef struct {
    FunctionParameterNode *parameters;
    StatementNode *statements;
} FunctionExpression;

typedef struct {
    ExpressionNode *arguments;
    Expression index;
} FunctionCallExpression;

typedef struct {
    Position position;
    Symbol *symbol;
    Expression initializer;
} TableNamedInitializerExpression;

typedef struct {
    Position position;
    Expression index;
    Expression initializer;
} TableIndexInitializerExpression;

typedef enum {
    TABLE_INITIALIZER_NONE = 0,

    TABLE_INITIALIZER_NAMED,
    TABLE_INITIALIZER_INDEX,
    TABLE_INITIALIZER_EXPRESSION,

} TableInitializerExpressionNodeType;

typedef struct TableInitializerExpressionNode {
    struct TableInitializerExpressionNode *next;
    TableInitializerExpressionNodeType type;
    void *value;
} TableInitializerExpressionNode;

typedef struct {
    TableInitializerExpressionNode *initializers;
} TableExpression;

typedef struct {
    Expression expression;
} CountExpression;

#endif //LUA_PARSER_EXPRESSION_STRUCTS_H
