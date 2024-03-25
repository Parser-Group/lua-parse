#include "expressions.h"
#include "position.h"

Expression expression_empty() {
    Expression exp = {0};
    return exp;
}

Expression binary_expression_parse(Parser *p, Expression left) {
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
            return expression_empty();
    }
    parser_consume(p);
    
    Expression right = expression_parse(p);
    
    Expression exp;
    exp.type = EXPRESSION_BINARY;
    exp.position = position_from_to(&left.position, &right.position);
    
    BinaryExpression binaryExp;
    binaryExp.type = (BinaryExpressionType)p->cur_token.type;
    binaryExp.left = &left;
    binaryExp.right = &right;
    binaryExp.parent = &exp;
    
    exp.value = &binaryExp;
    return exp;
}

Expression internal_expression_parse(Parser *p) {
    if (p->cur_token.type == TOKEN_SYMBOL) {
        Expression exp;
        
        VariableExpression varExp;
        varExp.text = p->cur_token.text;
        varExp.text_len = p->cur_token.text_len;
        varExp.parent = &exp;
        
        exp.type = EXPRESSION_VARIABLE;
        exp.position = p->cur_token.position;
        exp.value = &varExp;
        
        return exp;
    }
    
    
}

Expression expression_chain_parse(Parser *p, Expression first) {
    Expression exp;
    
    exp = binary_expression_parse(p, first);
    if (exp.type != EXPRESSION_NONE) {
        return exp;
    }
    
    if (p->cur_token.type == TOKEN_DOT) {
        parser_consume(p);
        
        if (p->cur_token.type == TOKEN_OPEN_BRACKET) {
            Token openBracket = p->cur_token;
            parser_consume(p);
            
            VariableIndex varExp;
            varExp.parent = &exp;
            varExp.first = &first;
            varExp.index = expression_parse(p);
            
            exp.type = EXPRESSION_VARIABLE_INDEX;
            exp.position = position_from_to(&first.position, &varExp.index.position);
            exp.value = &varExp;
            
            if (p->cur_token.type != TOKEN_CLOSE_BRACKET) {
                //TODO: print missing bracket
                printf("%s miss bracket", position_to_string(&openBracket.position));
            }
            
            return exp;
        }
        
        if (p->cur_token.type == TOKEN_SYMBOL) {
            VariableNameIndex varExp;
            varExp.parent = &exp;
            varExp.first = &first;
            varExp.index = p->cur_token.text;
            varExp.index_len = p->cur_token.text_len;
        
            exp.type = EXPRESSION_VARIABLE_NAME_INDEX;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = &varExp;
            
            return exp;
        }
    }
    
    return exp;
}

Expression expression_parse(Parser *p) {
    Expression exp = internal_expression_parse(p);
    if (exp.type == EXPRESSION_NONE) {
        return exp;
    }
    
    while (true) {
        Expression newExp = expression_chain_parse(p, exp);
        if (newExp.type == EXPRESSION_NONE) {
            return exp;
        }
        exp = newExp;
    }
}