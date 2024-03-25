#include "expressions.h"

Expression* binary_expression_parse(Parser *p, Expression *left) {
    switch (p->cur_token.type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        case TOKEN_POWER:
        case TOKEN_MODULO:

        case TOKEN_BIT_NOT:
        case TOKEN_BIT_OR:
        case TOKEN_BIT_AND:
        case TOKEN_BIT_SHIFT_LEFT:
        case TOKEN_BIT_SHIFT_RIGHT:

        case TOKEN_EQUALS:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_OR_EQUAL:
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_OR_EQUAL:
            
        case TOKEN_STRING_CONCAT:
            break;
        default:
            return nullptr;
    }

    parser_consume(p);
    
    Expression *right = expression_parse(p);
    
    Expression binaryExp = {0};
    binaryExp.type = EXPRESSION_BINARY;
    //binaryExp.position
    
    return 
}

Expression* internal_expression_parse(Parser *p) {
    
}

Expression* expression_chain_parse(Parser *p, Expression *first) {
    
}

Expression* expression_parse(Parser *p) {
    Expression *exp = internal_expression_parse(p);
    if (exp == nullptr) {
        return nullptr;
    }
    
    while (true) {
        Expression *newExp = expression_chain_parse(p, exp);
        if (newExp == nullptr) {
            return exp;
        }
        exp = newExp;
    }
}