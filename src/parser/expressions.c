#include "expressions.h"

#include <string.h>

#include "position.h"
#include "output.h"
#include "utils.h"

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
    parameter.name = symbol_from_token(&p->cur_token);
    return parameter;
}

Expression function_expression_parse(Parser *p, Token func) {
    Token openParen = p->cur_token;
    if (openParen.type != TOKEN_OPEN_PAREN) {
        const char *message = "missing open parenthesis in function parameter declaration";
        output_miss_open_paren(p, &func.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    FunctionParameterNode *parameterHead = malloc(sizeof(FunctionParameterNode));
    if (parameterHead == nullptr) {
        UNIMPLEMENTED("function_expression_parse");
    }
    
    parameterHead->value = nullptr;
    FunctionParameterNode *parameterCurrent = parameterHead;
    Position lastPos = {0};
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL || p->cur_token.type == TOKEN_KEYWORD) {
            FunctionParameter parameter = function_parameter_parse(p);

            FunctionParameterNode *node = malloc(sizeof(FunctionParameterNode));
            if (node == nullptr) {
                UNIMPLEMENTED("function_expression_parse");
            }
            
            node->value = &parameter;
            node->next = nullptr;

            parameterCurrent->next = node;
            parameterCurrent = node;
            parser_consume(p);
        }
        else if (lastPos.start_line != 0) {
            const char *message = "missing symbol for function expression as parameter";
            output_miss_symbol(p, &lastPos, message, strlen(message));
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
    
    StatementNode *statementHead = malloc(sizeof(StatementNode));
    if (statementHead == nullptr) {
        UNIMPLEMENTED("function_expression_parse");
    }
    
    statementHead->value = nullptr;
    StatementNode *statementCurrent = statementHead;
    while (true) {
        if (token_is_keyword("end", &p->cur_token) || p->cur_token.type == TOKEN_END) {
            break;    
        }
        
        Statement *statement = statement_parse(p);
        if (statement->type != STATEMENT_NONE) {
            StatementNode *node = malloc(sizeof(StatementNode));
            if (node == nullptr) {
                UNIMPLEMENTED("function_expression_parse");
            }
            
            node->value = statement;
            node->next = nullptr;
            
            statementCurrent->next = node;
            statementCurrent = node;
        }
        else {
            free(statement);
        }
    }
    
    if (token_is_keyword("end", &p->cur_token)) {
        lastPos = p->cur_token.position;
    }
    else if (statementCurrent->value != nullptr) {
        lastPos = statementCurrent->value->position;
    }
    else if (closeParen.type == TOKEN_CLOSE_PAREN) {
        lastPos = closeParen.position;
    }
    else if (parameterCurrent->value != nullptr) {
        lastPos = parameterCurrent->value->position;
    }
    else if (openParen.type == TOKEN_OPEN_PAREN) {
        lastPos = openParen.position;
    }
    else {
        lastPos = func.position;
    }
    
    if (p->cur_token.type != TOKEN_END) {
        const char *message = "missing \"end\" to signal end of function";
        output_miss_keyword_end(p, &func.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    Expression exp;
    
    FunctionExpression functionExpression;
    functionExpression.parent = &exp;
    functionExpression.position = position_from_to(&func.position, &lastPos);
    functionExpression.parameters = parameterHead->next;
    free(parameterHead);
    functionExpression.statements = statementHead->next;
    free(statementHead);
    
    exp.type = EXPRESSION_FUNCTION;
    exp.value = &functionExpression;
    exp.position = functionExpression.position;
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
    Token token = p->cur_token;

    if (token.type == TOKEN_OPEN_PAREN) {
        parser_consume(p);

        Expression expValue = expression_parse(p);
        
        Position lastPos;
        
        Token closeParen = p->cur_token;
        if (closeParen.type != TOKEN_CLOSE_PAREN) {
            const char *message = "missing close parenthesis for priority expression";
            output_miss_close_paren(p, &token.position, message, strlen(message));
            
            lastPos = expValue.position;
        }
        else {
            parser_consume(p);
            lastPos = closeParen.position;
        }

        Expression exp;

        PriorityExpression priorityExpression;
        priorityExpression.parent = &exp;
        priorityExpression.value = expValue;
        
        exp.type = EXPRESSION_PRIORITY;
        exp.value = &expValue;
        exp.position = position_from_to(&token.position, &lastPos);
        
        return exp;
    }
    
    if (token.type == TOKEN_SYMBOL) {
        parser_consume(p);
        
        Expression exp;
        
        VariableExpression varExp;
        varExp.symbol = symbol_from_token(&token);
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

    Token token = p->cur_token;
    if (token.type == TOKEN_OPEN_BRACKET) {
        Token openBracket = p->cur_token;
        parser_consume(p);

        Expression index = expression_parse(p);
        if (index.type == EXPRESSION_NONE) {
            output_miss_expression(p, &openBracket.position, "", 0);
        }

        if (p->cur_token.type != TOKEN_CLOSE_BRACKET) {
            output_miss_close_bracket(p, &openBracket.position, "", 0);
        }
        else {
            parser_consume(p);
        }
        
        VariableIndexExpression varExp;
        varExp.parent = &exp;
        varExp.first = &first;
        varExp.index = index;
        
        exp.type = EXPRESSION_VARIABLE_INDEX;
        exp.position = position_from_to(&first.position, &varExp.index.position);
        exp.value = &varExp;

        return exp;
    }
    
    if (token.type == TOKEN_DOT) {
        parser_consume(p);
        
        if (p->cur_token.type == TOKEN_SYMBOL) {
            VariableNameIndexExpression varExp;
            varExp.parent = &exp;
            varExp.first = &first;
            varExp.index = symbol_from_token(&p->cur_token);
        
            exp.type = EXPRESSION_VARIABLE_NAME_INDEX;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = &varExp;

            parser_consume(p);
            return exp;
        }
        
        const char *message = "missing symbol after dot";
        output_miss_symbol(p, &token.position, message, strlen(message));
        return expression_empty();
    }

    if (token.type == TOKEN_COLON) {
        parser_consume(p);

        if (p->cur_token.type == TOKEN_SYMBOL) {
            VariableNameIndexWithSelfExpression varExp;
            varExp.parent = &exp;
            varExp.first = &first;
            varExp.index = symbol_from_token(&p->cur_token);

            exp.type = EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = &varExp;

            parser_consume(p);
            return exp;
        }

        const char *message = "missing symbol after colon";
        output_miss_symbol(p, &token.position, message, strlen(message));
        return expression_empty();
    }

    if (token.type == TOKEN_OPEN_PAREN) {
        parser_consume(p);
        
        ExpressionNode *expressionHead = malloc(sizeof(ExpressionNode));
        if (expressionHead == nullptr) {
            UNIMPLEMENTED("expression_chain_parse");
        }
        
        expressionHead->value = nullptr;
        ExpressionNode *expressionCurrent = expressionHead;
        Position lastPos = {0};
        while (true) {
            Expression expValue = expression_parse(p);
            if (expValue.type != EXPRESSION_NONE) {
                ExpressionNode *node = malloc(sizeof(ExpressionNode));
                if (node == nullptr) {
                    UNIMPLEMENTED("expression_chain_parse");
                }

                node->value = &expValue;
                node->next = nullptr;
                
                expressionCurrent->next = node;
                expressionCurrent = node;
            }
            else if (lastPos.start_line != 0) {
                const char *message = "missing expression as function argument";
                output_miss_expression(p, &lastPos, message, strlen(message));
            }
            
            if (p->cur_token.type != TOKEN_COMMA) {
                break;
            }
            lastPos = p->cur_token.position;
            parser_consume(p);
        }

        if (p->cur_token.type != TOKEN_CLOSE_PAREN) {
            const char *message = "missing close parenthesis for function call";
            Position posAfter = position_after(&lastPos);
            output_miss_close_paren(p, &posAfter, message, strlen(message));
        }
        else {
            lastPos = p->cur_token.position;
            parser_consume(p);
        }
        
        Expression expression;
        
        FunctionCallExpression functionCallExpression;
        functionCallExpression.parent = &expression;
        functionCallExpression.arguments = expressionHead->next;
        free(expressionHead);
        functionCallExpression.index = first;
        
        expression.type = EXPRESSION_FUNCTION_CALL;
        expression.value = &functionCallExpression;
        expression.position = position_from_to(&first.position, &lastPos);
        return expression;
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