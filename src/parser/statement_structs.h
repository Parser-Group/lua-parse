#ifndef LUA_PARSER_STATEMENT_STRUCTS_H
#define LUA_PARSER_STATEMENT_STRUCTS_H

#include "position.h"
#include "expressions.h"
#include "expression_structs.h"
#include "statement_struct.h"
#include "symbol.h"

typedef struct {
    StatementNode *statements;
} DoStatement;

typedef struct {
    Statement child;
} LocalStatement;

typedef struct {
    Symbol *symbol;
    Expression *initializer;
} VariableDeclaration;

typedef struct {
    Expression expression;
    Expression index;
} FunctionDeclaration;

typedef struct {
    Expression expression;
} ReturnStatement;

typedef struct {
    Expression expression;
    Expression index;
} AssignmentStatement;

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
    Expression condition;
    StatementNode *if_body;
    ElseIfStatementNode *else_if_body_nodes;
    ElseStatementNode *else_body_nodes;
} IfStatement;

typedef struct {
    Symbol *symbol;
    Expression initializer;
    Expression condition;
    Expression increment;
    StatementNode *statements;
} ForNumericLoopStatement;

typedef struct {
    SymbolNode *symbols;
    Expression get_iterator;
    StatementNode *statements;
} ForGenericLoopStatement;

typedef struct {
    StatementNode *statements;
    Expression condition;
} RepeatStatement;

typedef struct {
    Expression condition;
    StatementNode *statements;
} WhileStatement;

#endif //LUA_PARSER_STATEMENT_STRUCTS_H
