#ifndef LUA_PARSER_VARIABLE_DECLARATION_H
#define LUA_PARSER_VARIABLE_DECLARATION_H

#include "parser.h"

typedef struct {
    Token name;
    Expression initializer;
} VariableDeclaration;

VariableDeclaration parse_variable_declaration(Parser *p, Token name) {
    VariableDeclaration var;
    var.name = name;
    
    Token token = lexer_next(p->lexer);
    
    if (token.type == TOKEN_EQUAL) {
        var.initializer = parse_expression(p);
    }
    
    return var;
}

#endif //LUA_PARSER_VARIABLE_DECLARATION_H
