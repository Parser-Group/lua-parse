#ifndef LUA_PARSER_STATEMENT_STRUCTS_H
#define LUA_PARSER_STATEMENT_STRUCTS_H

#include "position.h"
#include "expressions.h"
#include "expression_structs.h"
#include "statement_struct.h"

typedef struct {
    Statement *parent;
    Expression expression;
} ExpressionStatement;

typedef struct {
    Statement *parent;
    Statement *child;
} LocalStatement;

typedef struct {
    Statement *parent;
    const char *symbol;
    size_t symbol_len;
    Expression *initializer;
} VariableDeclaration;

typedef struct {
    Statement *parent;
    Expression expression;
    const char *symbol;
    size_t symbol_len;
} FunctionDeclaration;

typedef struct {
    Statement *parent;
    Expression expression;
    const char *symbol;
    size_t symbol_len;
} VariableAssignmentStatement;

typedef struct {
    Expression condition;
    StatementNode *statements;
} ElseIfStatement;

typedef struct {
    StatementNode *statements;
} ElseStatement;

typedef struct ElseIfStatementNode {
    struct ElseIfStatementNode *next;
    ElseIfStatement *value;
} ElseIfStatementNode;

typedef struct ElseStatementNode {
    struct ElseStatementNode *next;
    ElseStatement *value;
} ElseStatementNode;

typedef struct {
    Statement *parent;
    Expression condition;
    StatementNode *if_body;
    ElseIfStatementNode *else_if_body_nodes;
    ElseStatementNode *else_body_nodes;
} IfStatement;

#endif //LUA_PARSER_STATEMENT_STRUCTS_H
