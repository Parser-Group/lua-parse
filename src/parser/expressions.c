#include "expressions.h"

#include <string.h>

#include "position.h"
#include "output.h"

Expression expression_empty() {
    Expression exp = {0};
    return exp;
}

FunctionParameter function_parameter_parse(Parser *p) {
    if (p->cur_token.type == TOKEN_KEYWORD) {
        const char *message = "unexpected keyword in parameter";
        output_unexpected_keyword(p, &p->cur_token.position, message, strlen(message));
    }
    
    FunctionParameter parameter;
    parameter.position = p->cur_token.position;
    parameter.text = p->cur_token.text;
    parameter.text_len = p->cur_token.text_len;
    
    return parameter;
}

FunctionExpression function_expression_parse(Parser *p, Token func) {
    Token openParen = p->cur_token;
    if (openParen.type != TOKEN_OPEN_PAREN) {
        const char *message = "missing open parenthesis in function parameter declaration";
        output_miss_open_paren(p, &func.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    FunctionParameterNode parameterHead = {0};
    FunctionParameterNode parameterCurrent = parameterHead;
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL || p->cur_token.type == TOKEN_KEYWORD) {
            FunctionParameter parameter = function_parameter_parse(p);

            FunctionParameterNode node;
            node.value = &parameter;

            parameterCurrent.next = &node;
            parameterCurrent = node;
            parser_consume(p);
        }
        
        if (p->cur_token.type == TOKEN_COMMA) {
            parser_consume(p);
            continue;
        }
        
        break;
    }
    
    Token closeParen = p->cur_token;
    if (closeParen.type != TOKEN_CLOSE_PAREN && openParen.type == TOKEN_OPEN_PAREN) {
        const char *message = "missing close parenthesis in function parameter declaration";
        output_miss_close_paren(p, &openParen.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    StatementNode statementHead = {0};
    StatementNode statementCurrent = statementHead;
    while (true) {
        if (token_is_keyword("end", &p->cur_token)) {
            break;    
        }
        
        Statement statement = statement_parse(p);
        if (statement.type != STATEMENT_NONE) {
            StatementNode node;
            node.value = &statement;
            
            statementCurrent.next = &node;
            statementCurrent = node;
        }
    }
    
    Position lastPos;
    if (token_is_keyword("end", &p->cur_token)) {
        lastPos = p->cur_token.position;
    }
    else if (statementCurrent.value != NULL) {
        lastPos = statementCurrent.value->position;
    }
    else if (closeParen.type == TOKEN_CLOSE_PAREN) {
        lastPos = closeParen.position;
    }
    else if (parameterCurrent.value != NULL) {
        lastPos = parameterCurrent.value->position;
    }
    else if (openParen.type == TOKEN_OPEN_PAREN) {
        lastPos = openParen.position;
    }
    else {
        lastPos = func.position;
    }
    
    if (p->cur_token.type != TOKEN_END) {
        const char *message = "missing \"end\" to signal end of function";
        output_miss_end(p, &func.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    FunctionExpression functionExpression;
    functionExpression.position = position_from_to(&func.position, &lastPos);
    functionExpression.parameters = parameterHead.next;
    functionExpression.statements = statementHead.next;
    return functionExpression;
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
    Token token = p->cur_token;

    if (token.type == TOKEN_OPEN_PAREN) {
        parser_consume(p);

        Expression exp = expression_parse(p);

        if (p->cur_token.type != TOKEN_CLOSE_PAREN) {
            output_miss_close_paren(p, &token.position, "", 0);
        }
        else {
            parser_consume(p);
        }

        return exp;
    }
    
    if (token.type == TOKEN_SYMBOL) {
        Expression exp;
        
        VariableExpression varExp;
        varExp.text = token.text;
        varExp.text_len = token.text_len;
        varExp.parent = &exp;
        
        exp.type = EXPRESSION_VARIABLE;
        exp.position = token.position;
        exp.value = &varExp;
        
        return exp;
    }
 
    // literals
    if (token.type == TOKEN_NUMBER) {
        parser_consume(p);
        
        Expression exp;
        
        LiteralNumberExpression numberLiteral;
        numberLiteral.parent = &exp;
        numberLiteral.value = strtod(token.text, NULL);
        
        exp.position = token.position;
        exp.type = EXPRESSION_LITERAL_NUMBER;
        exp.value = &numberLiteral;
        return exp;
    }
    if (token.type == TOKEN_STRING) {
        parser_consume(p);

        Expression exp;

        LiteralStringExpression strLiteral;
        strLiteral.parent = &exp;
        strLiteral.value = token.text;
        strLiteral.value_len = token.text_len;
        
        exp.position = token.position;
        exp.type = EXPRESSION_LITERAL_STRING;
        exp.value = &strLiteral;
        return exp;
    }
    
    return expression_empty();
}

Expression expression_chain_parse(Parser *p, Expression first) {
    Expression exp;
    
    exp = binary_expression_parse(p, first);
    if (exp.type != EXPRESSION_NONE) {
        return exp;
    }

    if (p->cur_token.type == TOKEN_OPEN_BRACKET) {
        Token openBracket = p->cur_token;
        parser_consume(p);

        VariableIndexExpression varExp;
        varExp.parent = &exp;
        varExp.first = &first;
        varExp.index = expression_parse(p);
        
        exp.type = EXPRESSION_VARIABLE_INDEX;
        exp.position = position_from_to(&first.position, &varExp.index.position);
        exp.value = &varExp;

        if (varExp.index.type == EXPRESSION_NONE) {
            output_miss_expression(p, &openBracket.position, "", 0);
        }
        
        if (p->cur_token.type != TOKEN_CLOSE_BRACKET) {
            output_miss_close_bracket(p, &openBracket.position, "", 0);
        }
        else {
            parser_consume(p);
        }

        return exp;
    }
    
    if (p->cur_token.type == TOKEN_DOT) {
        Token dot = p->cur_token;
        parser_consume(p);
        
        if (p->cur_token.type == TOKEN_SYMBOL) {
            VariableNameIndexExpression varExp;
            varExp.parent = &exp;
            varExp.first = &first;
            varExp.index = p->cur_token.text;
            varExp.index_len = p->cur_token.text_len;
        
            exp.type = EXPRESSION_VARIABLE_NAME_INDEX;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = &varExp;
            
            return exp;
        }

        output_miss_symbol(p, &dot.position, "", 0);
        return expression_empty();
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