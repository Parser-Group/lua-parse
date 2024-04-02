#include "expressions.h"

#include <string.h>

#include "position.h"
#include "output.h"
#include "utils.h"

Expression expression_empty() {
    Expression exp = {0};
    return exp;
}

FunctionParameter* function_parameter_parse(Parser *p) {
    if (p->cur_token.type == TOKEN_KEYWORD) {
        const char *message = "unexpected keyword in parameter";
        output_unexpected_keyword(p, &p->cur_token.position, message, strlen(message));
    }

    SAFE_MALLOC(FunctionParameter, parameter)
    parameter->position = p->cur_token.position;
    parameter->name = symbol_from_token(&p->cur_token);
    return parameter;
}

Expression function_expression_parse(Parser *p, Token *func) {
    Token openParen = p->cur_token;
    if (openParen.type != TOKEN_OPEN_PAREN) {
        parser_rollback(p);
        return expression_empty();
    }
    else {
        parser_consume(p);
    }
    
    SAFE_MALLOC(FunctionParameterNode, parameterHead)
    parameterHead->next = NULL;
    parameterHead->value = NULL;
    FunctionParameterNode *parameterCurrent = parameterHead;
    SAFE_MALLOC(Position , lastPos);
    lastPos->start_line = 0;
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL || p->cur_token.type == TOKEN_KEYWORD) {
            FunctionParameter *parameter = function_parameter_parse(p);

            SAFE_MALLOC(FunctionParameterNode, node)
            
            node->value = parameter;
            node->next = NULL;

            parameterCurrent->next = node;
            parameterCurrent = node;
            parser_consume(p);
            
            lastPos = &parameter->position;
        }
        else if (lastPos->start_line != 0) {
            const char *message = "missing symbol for function expression as parameter";
            output_miss_symbol(p, lastPos, message, strlen(message));
        }
        
        if (p->cur_token.type == TOKEN_COMMA) {
            parser_consume(p);
            lastPos = memcpy(lastPos, &p->cur_token.position, sizeof(Position));
            continue;
        }
        
        break;
    }
    
    Token closeParen = p->cur_token;
    if (closeParen.type != TOKEN_CLOSE_PAREN) {
        const char *message = "missing close parenthesis in function parameter declaration";
        output_miss_close_paren(p, &openParen.position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    SAFE_MALLOC(StatementNode, statementHead)
    statementHead->next = NULL;
    statementHead->value = (Statement){0};
    StatementNode *statementCurrent = statementHead;
    while (true) {
        if (token_is_keyword("end", &p->cur_token) || p->cur_token.type == TOKEN_END) {
            break;    
        }
        
        Statement statement = statement_parse(p);
        if (statement.type != STATEMENT_NONE) {
            SAFE_MALLOC(StatementNode, node)
            node->next = NULL;
            node->value = statement;
            
            statementCurrent->next = node;
            statementCurrent = node;
        }
    }
    
    Token end = p->cur_token;
    if (!token_is_keyword("end", &end)) {
        const char *message = "missing \"end\" to signal end of function";
        output_miss_keyword_end(p, &func->position, message, strlen(message));
    }
    else {
        parser_consume(p);
    }
    
    if (token_is_keyword("end", &end)) {
        lastPos = &end.position;
    }
    else if (statementCurrent->value.type != STATEMENT_NONE) {
        lastPos = &statementCurrent->value.position;
    }
    else if (closeParen.type == TOKEN_CLOSE_PAREN) {
        lastPos = &closeParen.position;
    }
    else if (parameterCurrent->value != NULL) {
        lastPos = &parameterCurrent->value->position;
    }
    else {
        lastPos = &openParen.position;
    }
    
    Expression exp;

    SAFE_MALLOC(FunctionExpression, functionExpression)
    functionExpression->parameters = parameterHead->next;
    free(parameterHead);
    functionExpression->statements = statementHead->next;
    free(statementHead);
    
    exp.type = EXPRESSION_FUNCTION;
    exp.value = &functionExpression;
    exp.position = position_from_to(&func->position, lastPos);
    return exp;
}

Expression expression_table_parse(Parser *p, Token *openBrace) {
    SAFE_MALLOC(TableInitializerExpressionNode, initializerExpressionsHead)

    initializerExpressionsHead->value = NULL;
    initializerExpressionsHead->next = NULL;

    TableInitializerExpressionNode *initializerExpressionsCurrent = initializerExpressionsHead;
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL) {
            Symbol *symbol = symbol_from_token(&p->cur_token);
            parser_consume(p);

            Position *lastPos = &symbol->position;
            if (p->cur_token.type != TOKEN_EQUAL) {
                const char *message = "missing \"=\" (equal) sign after symbol";
                output_miss_equal(p, lastPos, message, strlen(message));
            } else {
                lastPos = &p->cur_token.position;
                parser_consume(p);
            }

            Expression initializer = expression_parse(p);
            if (initializer.type == EXPRESSION_NONE) {
                const char *message = "missing initializer for symbol in table";
                output_miss_expression(p, lastPos, message, strlen(message));
            }

            SAFE_MALLOC(TableInitializerExpressionNode, node)

            TableNamedInitializerExpression expression;

            expression.symbol = symbol;
            expression.initializer = initializer;
            expression.position = position_from_to(&symbol->position, &initializer.position);

            node->type = TABLE_INITIALIZER_NAMED;
            node->value = &expression;
            node->next = NULL;

            initializerExpressionsCurrent->next = node;
            initializerExpressionsCurrent = node;
            goto afterInitializerExpression;
        }

        if (p->cur_token.type == TOKEN_OPEN_BRACKET) {
            Token openBracket = p->cur_token;
            parser_consume(p);

            Position *lastPos = &openBracket.position;
            Expression index = expression_parse(p);
            if (index.type == EXPRESSION_NONE) {
                const char *message = "missing index expression in table initialization";
                output_miss_expression(p, &openBracket.position, message, strlen(message));
            } else {
                lastPos = &index.position;
            }

            if (p->cur_token.type != TOKEN_CLOSE_BRACKET) {
                const char *message = "missing closing bracket after index expression";
                output_miss_close_bracket(p, lastPos, message, strlen(message));
            } else {
                lastPos = &p->cur_token.position;
                parser_consume(p);
            }

            if (p->cur_token.type != TOKEN_EQUAL) {
                const char *message = "missing \"=\" (equal) sign after index";
                output_miss_equal(p, lastPos, message, strlen(message));
            } else {
                lastPos = &p->cur_token.position;
                parser_consume(p);
            }

            Expression initializer = expression_parse(p);
            if (initializer.type == EXPRESSION_NONE) {
                const char *message = "missing initializer for symbol in table";
                output_miss_expression(p, lastPos, message, strlen(message));
            }

            SAFE_MALLOC(TableInitializerExpressionNode, node)

            TableIndexInitializerExpression expression;

            expression.index = index;
            expression.initializer = initializer;
            expression.position = position_from_to(&openBracket.position, &initializer.position);

            node->type = TABLE_INITIALIZER_INDEX;
            node->value = &expression;
            node->next = NULL;

            initializerExpressionsCurrent->next = node;
            initializerExpressionsCurrent = node;
            goto afterInitializerExpression;
        }

        Expression expression = expression_parse(p);
        if (expression.type != EXPRESSION_NONE) {
            SAFE_MALLOC(TableInitializerExpressionNode, node)

            node->type = TABLE_INITIALIZER_EXPRESSION;
            node->value = &expression;
            node->next = NULL;

            initializerExpressionsCurrent->next = node;
            initializerExpressionsCurrent = node;
            goto afterInitializerExpression;
        }

        afterInitializerExpression:
        if (p->cur_token.type == TOKEN_COMMA) {
            parser_consume(p);
            continue;
        }

        if (p->cur_token.type == TOKEN_CLOSE_BRACE) {
            break;
        }

        output_unexpected_token(p, &p->cur_token, "unexpected token in table initializer: %s");
        parser_consume(p);
    }
    Token closeBrace = p->cur_token;
    parser_consume(p);

    Expression expression;

    TableExpression tableExpression;
    tableExpression.initializers = initializerExpressionsHead->next;
    free(initializerExpressionsHead);

    expression.type = EXPRESSION_TABLE;
    expression.value = &tableExpression;
    expression.position = position_from_to(&openBrace->position, &closeBrace.position);

    return expression;
}

Expression binary_expression_parse(Parser *p, Expression left) {
    int addToType = 0;
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
        case TOKEN_NOT_EQUALS:
        case TOKEN_GREATER_THAN:
        case TOKEN_GREATER_THAN_OR_EQUAL:
        case TOKEN_LESS_THAN:
        case TOKEN_LESS_THAN_OR_EQUAL:
            
        case TOKEN_STRING_CONCAT:
            break;

        case TOKEN_KEYWORD:
            if (token_is_keyword("or", &p->cur_token)) {
                break;
            }
            if (token_is_keyword("and", &p->cur_token)) {
                ++addToType;
                break;
            }
            
        default:
            return expression_empty();
    }
    parser_consume(p);
    
    Expression right = expression_parse(p);
    
    Expression exp;
    exp.type = EXPRESSION_BINARY;
    exp.position = position_from_to(&left.position, &right.position);

    SAFE_MALLOC(BinaryExpression, binaryExp)
    binaryExp->type = (BinaryExpressionType)p->cur_token.type + addToType;
    binaryExp->left = left;
    binaryExp->right = right;
    
    exp.value = &binaryExp;
    return exp;
}

Expression internal_expression_parse(Parser *p) {
    Token token = p->cur_token;
    
    if (token.type == TOKEN_OPEN_PAREN) {
        parser_consume(p);

        Expression expValue = expression_parse(p);
        
        Position *lastPos;
        
        Token closeParen = p->cur_token;
        if (closeParen.type != TOKEN_CLOSE_PAREN) {
            const char *message = "missing close parenthesis for priority expression";
            output_miss_close_paren(p, &token.position, message, strlen(message));
            
            lastPos = &expValue.position;
        }
        else {
            parser_consume(p);
            lastPos = &closeParen.position;
        }

        Expression exp;

        PriorityExpression priorityExpression;
        priorityExpression.value = expValue;
        
        exp.type = EXPRESSION_PRIORITY;
        exp.value = &expValue;
        exp.position = position_from_to(&token.position, lastPos);
        
        return exp;
    }

    if (token.type == TOKEN_HASH) {
        parser_consume(p);

        Position *lastPos = &token.position;
        Expression valueExpression = expression_parse(p);
        if (valueExpression.type == EXPRESSION_NONE) {
            const char *message = "missing expression after count operator";
            output_miss_expression(p, &token.position, message, strlen(message));
        } else {
            lastPos = &valueExpression.position;
        }

        Expression expression;

        CountExpression countExpression;
        countExpression.expression = valueExpression;

        expression.type = EXPRESSION_COUNT;
        expression.value = &countExpression;
        expression.position = position_from_to(&token.position, lastPos);

        return expression;
    }
    
    if (token.type == TOKEN_SYMBOL) {
        parser_consume(p);
        
        Expression exp;
        
        VariableExpression varExp;
        varExp.symbol = symbol_from_token(&token);
        
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
        strLiteral.value = token.text;
        strLiteral.value_len = token.text_len;
        
        exp.position = token.position;
        exp.type = EXPRESSION_LITERAL_STRING;
        exp.value = &strLiteral;
        return exp;
    }

    if (token.type == TOKEN_OPEN_BRACE) {
        parser_consume(p);
        
        return expression_table_parse(p, &token);
    }

    if (token.type == TOKEN_BIT_NOT) {
        parser_consume(p);
        Position *lastPos = &token.position;
        
        Expression valueExpression = expression_parse(p);
        if (valueExpression.type == EXPRESSION_NONE) {
            const char *message = "missing expression after not bit \"~\" operator";
            output_miss_expression(p, lastPos, message, strlen(message));
        } else {
            lastPos = &valueExpression.position;
        }
        
        Expression expression;
        
        BitNotExpression bitNotExpression;
        bitNotExpression.value = valueExpression;
        
        expression.type = EXPRESSION_BIT_NOT;
        expression.position = position_from_to(&token.position, lastPos);
        expression.value = &bitNotExpression;
        return expression;
    }

    if (token_is_keyword("nil", &token)) {
        parser_consume(p);
        
        Expression expression;
        expression.type = EXPRESSION_LITERAL_NIL;
        expression.position = token.position;
        expression.value = NULL;
        return expression;
    }
    
    if (token_is_keyword("true", &token)) {
        parser_consume(p);
        
        Expression expression;
        
        LiteralBooleanExpression booleanExpression;
        booleanExpression.value = true;
        
        expression.position = token.position;
        expression.type = EXPRESSION_LITERAL_BOOLEAN;
        expression.value = &booleanExpression;
        return expression;
    }

    if (token_is_keyword("false", &token)) {
        parser_consume(p);

        Expression expression;

        LiteralBooleanExpression booleanExpression;
        booleanExpression.value = false;

        expression.position = token.position;
        expression.type = EXPRESSION_LITERAL_BOOLEAN;
        expression.value = &booleanExpression;
        return expression;
    }

    if (token_is_keyword("not", &token)) {
        parser_consume(p);
        
        Position *lastPos = &token.position;
        Expression valueExpression = expression_parse(p);
        if (valueExpression.type == EXPRESSION_NONE) {
            const char *message = "missing expression after \"not\" keyword / operator";
            output_miss_expression(p, &token.position, message, strlen(message));
        } else {
            lastPos = &valueExpression.position;
        }
        
        Expression expression;
        
        NotExpression notExpression;
        notExpression.value = valueExpression;
        
        expression.position = position_from_to(&token.position, lastPos);
        expression.type = EXPRESSION_NOT;
        expression.value = &notExpression;
        return expression;
    }
    
    if (token_is_keyword("function", &token)) {
        parser_consume(p);
        
        return function_expression_parse(p, &token);
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
        parser_consume(p);

        Expression index = expression_parse(p);
        if (index.type == EXPRESSION_NONE) {
            const char *message = "missing index expression";
            output_miss_expression(p, &token.position, message, strlen(message));
        }

        if (p->cur_token.type != TOKEN_CLOSE_BRACKET) {
            const char *message = "missing close bracket after index expression";
            output_miss_close_bracket(p, &token.position, message, strlen(message));
        }
        else {
            parser_consume(p);
        }
        
        VariableIndexExpression varExp;
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
            SAFE_MALLOC(VariableNameIndexExpression, varExp)
            varExp->first = &first;
            varExp->index = symbol_from_token(&p->cur_token);
        
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
            SAFE_MALLOC(VariableNameIndexWithSelfExpression, varExp)
            varExp->first = &first;
            varExp->index = symbol_from_token(&p->cur_token);

            exp.type = EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = varExp;

            parser_consume(p);
            return exp;
        }

        const char *message = "missing symbol after colon";
        output_miss_symbol(p, &token.position, message, strlen(message));
        return expression_empty();
    }

    if (token.type == TOKEN_OPEN_PAREN) {
        parser_consume(p);
        
        SAFE_MALLOC(ExpressionNode, expressionHead)
        expressionHead->value = NULL;
        ExpressionNode *expressionCurrent = expressionHead;
        Position *lastPos = NULL;
        while (true) {
            Expression expValue = expression_parse(p);
            if (expValue.type != EXPRESSION_NONE) {
                SAFE_MALLOC(ExpressionNode, node)

                node->value = &expValue;
                node->next = NULL;
                
                expressionCurrent->next = node;
                expressionCurrent = node;
            }
            else if (lastPos != NULL && lastPos->start_line != 0) {
                const char *message = "missing expression as function argument";
                output_miss_expression(p, lastPos, message, strlen(message));
            }
            
            if (p->cur_token.type != TOKEN_COMMA) {
                break;
            }
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }

        if (p->cur_token.type != TOKEN_CLOSE_PAREN) {
            const char *message = "missing close parenthesis for function call";
            Position posAfter = position_after(lastPos);
            output_miss_close_paren(p, &posAfter, message, strlen(message));
        }
        else {
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }
        
        Expression expression;
        
        FunctionCallExpression functionCallExpression;
        functionCallExpression.arguments = expressionHead->next;
        free(expressionHead);
        functionCallExpression.index = first;
        
        expression.type = EXPRESSION_FUNCTION_CALL;
        expression.value = &functionCallExpression;
        expression.position = position_from_to(&first.position, lastPos);
        return expression;
    }

//    if (token_is_keyword("and", &token)) {
//        parser_consume(p);
//        
//        Expression right = expression_parse(p);
//        if (right.type == EXPRESSION_NONE) {
//            const char *message = "missing right side of expression";
//        }
//        
//    }
    
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