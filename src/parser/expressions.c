#include "expressions.h"

#include <string.h>

#include "position.h"
#include "output.h"
#include "utils.h"

Expression expression_empty() {
    Expression exp = {0};
    return exp;
}

FunctionParameter *function_parameter_parse(Parser *p) {
    if (p->cur_token.type == TOKEN_KEYWORD) {
        const char *message = "unexpected keyword in parameter";
        output_unexpected_keyword(p, &p->cur_token.position, message, strlen(message));
    }

    SAFE_MALLOC(FunctionParameter, parameter)
    parameter->position = p->cur_token.position;
    parameter->name = p->cur_token;
    return parameter;
}

Expression function_expression_parse(Parser *p, Token *func) {
    Token openParen = p->cur_token;
    if (openParen.type != TOKEN_OPEN_PAREN) {
        parser_rollback(p);
        return expression_empty();
    } else {
        parser_consume(p);
    }

    SAFE_MALLOC(FunctionParameterNode, parameterHead)
    parameterHead->next = NULL;
    parameterHead->value = NULL;
    FunctionParameterNode *parameterCurrent = parameterHead;
    Position lastPos = {0};
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL || p->cur_token.type == TOKEN_KEYWORD) {
            FunctionParameter *parameter = function_parameter_parse(p);

            SAFE_MALLOC(FunctionParameterNode, node)

            node->value = parameter;
            node->next = NULL;

            parameterCurrent->next = node;
            parameterCurrent = node;
            parser_consume(p);

            lastPos = parameter->position;
        } else if (lastPos.start_line != 0) {
            const char *message = "missing symbol for function expression as parameter";
            output_miss_symbol(p, &lastPos, message, strlen(message));
        }

        if (p->cur_token.type == TOKEN_COMMA) {
            parser_consume(p);
            lastPos = p->cur_token.position;
            continue;
        }

        break;
    }

    Token closeParen = p->cur_token;
    if (closeParen.type != TOKEN_CLOSE_PAREN) {
        const char *message = "missing close parenthesis in function parameter declaration";
        output_miss_close_paren(p, &openParen.position, message, strlen(message));
    } else {
        parser_consume(p);
    }

    SAFE_MALLOC(StatementNode, statementHead)
    statementHead->next = NULL;
    statementHead->value = (Statement) {0};
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
    } else {
        parser_consume(p);
    }

    if (token_is_keyword("end", &end)) {
        lastPos = end.position;
    } else if (statementCurrent->value.type != STATEMENT_NONE) {
        lastPos = statementCurrent->value.position;
    } else if (closeParen.type == TOKEN_CLOSE_PAREN) {
        lastPos = closeParen.position;
    } else if (parameterCurrent->value != NULL) {
        lastPos = parameterCurrent->value->position;
    } else {
        lastPos = openParen.position;
    }
    
    Expression exp;

    SAFE_MALLOC(FunctionExpression, functionExpression)
    functionExpression->parameters = parameterHead->next;
    free(parameterHead);
    functionExpression->statements = statementHead->next;
    free(statementHead);

    exp.type = EXPRESSION_FUNCTION;
    exp.value = functionExpression;
    exp.position = position_from_to(&func->position, &lastPos);
    return exp;
}

Expression expression_table_parse(Parser *p, Token *openBrace) {
    SAFE_MALLOC(TableInitializerExpressionNode, initializerExpressionsHead)

    initializerExpressionsHead->value = NULL;
    initializerExpressionsHead->next = NULL;

    TableInitializerExpressionNode *initializerExpressionsCurrent = initializerExpressionsHead;
    while (true) {
        if (p->cur_token.type == TOKEN_SYMBOL) {
            Token symbol = p->cur_token;
            parser_consume(p);

            Position *lastPos = &symbol.position;
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

            SAFE_MALLOC(TableNamedInitializerExpression, tableNamedInitializerExpression)
            tableNamedInitializerExpression->symbol = symbol;
            tableNamedInitializerExpression->initializer = initializer;
            tableNamedInitializerExpression->position = position_from_to(&symbol.position, &initializer.position);

            node->type = TABLE_INITIALIZER_NAMED;
            node->value = tableNamedInitializerExpression;
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

            SAFE_MALLOC(TableIndexInitializerExpression, expression)

            expression->index = index;
            expression->initializer = initializer;
            expression->position = position_from_to(&openBracket.position, &initializer.position);

            node->type = TABLE_INITIALIZER_INDEX;
            node->value = expression;
            node->next = NULL;

            initializerExpressionsCurrent->next = node;
            initializerExpressionsCurrent = node;
            goto afterInitializerExpression;
        }

        Expression expression = expression_parse(p);
        if (expression.type != EXPRESSION_NONE) {
            SAFE_MALLOC(TableInitializerExpressionNode, node)

            SAFE_MALLOC(Expression, expressionHeap)
            memcpy(expressionHeap, &expression, sizeof(Expression));
            
            node->type = TABLE_INITIALIZER_EXPRESSION;
            node->value = expressionHeap;
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

    SAFE_MALLOC(TableExpression, tableExpression)
    tableExpression->initializers = initializerExpressionsHead->next;
    free(initializerExpressionsHead);

    expression.type = EXPRESSION_TABLE;
    expression.value = tableExpression;
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


    SAFE_MALLOC(BinaryExpression, binaryExp)
    binaryExp->type = (BinaryExpressionType) p->cur_token.type + addToType;
    binaryExp->left = left;
    binaryExp->right = right;

    Expression exp;
    exp.type = EXPRESSION_BINARY;
    exp.position = position_from_to(&left.position, &right.position);
    exp.value = binaryExp;
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
        } else {
            parser_consume(p);
            lastPos = &closeParen.position;
        }

        Expression exp;

        SAFE_MALLOC(PriorityExpression, priorityExpression)
        priorityExpression->value = expValue;

        exp.type = EXPRESSION_PRIORITY;
        exp.value = priorityExpression;
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

        SAFE_MALLOC(CountExpression, countExpression)
        countExpression->expression = valueExpression;

        expression.type = EXPRESSION_COUNT;
        expression.value = countExpression;
        expression.position = position_from_to(&token.position, lastPos);

        return expression;
    }

    if (token.type == TOKEN_SYMBOL) {
        parser_consume(p);

        Expression exp;

        SAFE_MALLOC(VariableExpression, varExp)
        varExp->symbol = token;

        exp.type = EXPRESSION_VARIABLE;
        exp.position = token.position;
        exp.value = varExp;

        return exp;
    }

    // literals
    if (token.type == TOKEN_NUMBER) {
        parser_consume(p);

        Expression exp;

        SAFE_MALLOC(LiteralNumberExpression, numberLiteral)
        numberLiteral->value = strtod(token.text, NULL);

        exp.position = token.position;
        exp.type = EXPRESSION_LITERAL_NUMBER;
        exp.value = numberLiteral;
        return exp;
    }

    if (token.type == TOKEN_STRING) {
        parser_consume(p);

        Expression exp;

        SAFE_MALLOC(LiteralStringExpression, strLiteral)
        strLiteral->value = token.text;
        strLiteral->value_len = token.text_len;

        exp.position = token.position;
        exp.type = EXPRESSION_LITERAL_STRING;
        exp.value = strLiteral;
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

        SAFE_MALLOC(BitNotExpression, bitNotExpression)
        bitNotExpression->value = valueExpression;

        expression.type = EXPRESSION_BIT_NOT;
        expression.position = position_from_to(&token.position, lastPos);
        expression.value = bitNotExpression;
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

        SAFE_MALLOC(LiteralBooleanExpression, boolean)
        boolean->value = true;

        expression.position = token.position;
        expression.type = EXPRESSION_LITERAL_BOOLEAN;
        expression.value = boolean;
        return expression;
    }

    if (token_is_keyword("false", &token)) {
        parser_consume(p);

        Expression expression;

        SAFE_MALLOC(LiteralBooleanExpression, booleanExpression)
        booleanExpression->value = false;

        expression.position = token.position;
        expression.type = EXPRESSION_LITERAL_BOOLEAN;
        expression.value = booleanExpression;
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

        SAFE_MALLOC(NotExpression, notExpression)
        notExpression->value = valueExpression;

        expression.position = position_from_to(&token.position, lastPos);
        expression.type = EXPRESSION_NOT;
        expression.value = notExpression;
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
        } else {
            parser_consume(p);
        }

        SAFE_MALLOC(VariableIndexExpression, varExp)
        varExp->first = first;
        varExp->index = index;

        exp.type = EXPRESSION_VARIABLE_INDEX;
        exp.position = position_from_to(&first.position, &varExp->index.position);
        exp.value = varExp;

        return exp;
    }

    if (token.type == TOKEN_DOT) {
        parser_consume(p);

        if (p->cur_token.type == TOKEN_SYMBOL) {
            SAFE_MALLOC(VariableNameIndexExpression, varExp)
            varExp->first = first;
            varExp->index = p->cur_token;

            exp.type = EXPRESSION_VARIABLE_NAME_INDEX;
            exp.position = position_from_to(&first.position, &p->cur_token.position);
            exp.value = varExp;

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
            varExp->first = first;
            varExp->index = p->cur_token;

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

        SAFE_MALLOC(ExpressionNode, argumentsExpressionHead)
        argumentsExpressionHead->next = NULL;
        argumentsExpressionHead->value = NULL;
        ExpressionNode *argumentsExpressionCurrent = argumentsExpressionHead;
        Position *lastPos = NULL;
        while (true) {
            Expression expValue = expression_parse(p);
            if (expValue.type != EXPRESSION_NONE) {
                SAFE_MALLOC(Expression, expValueHeap)
                memcpy(expValueHeap, &expValue, sizeof(Expression));
                
                SAFE_MALLOC(ExpressionNode, node)
                node->value = expValueHeap;
                node->next = NULL;

                argumentsExpressionCurrent->next = node;
                argumentsExpressionCurrent = node;
            } else if (lastPos != NULL && lastPos->start_line != 0) {
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
        } else {
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }

        Expression expression;

        SAFE_MALLOC(FunctionCallExpression, functionCallExpression)
        functionCallExpression->arguments = argumentsExpressionHead->next;
        free(argumentsExpressionHead);
        functionCallExpression->index = first;

        expression.type = EXPRESSION_FUNCTION_CALL;
        expression.value = functionCallExpression;
        expression.position = position_from_to(&first.position, lastPos);
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

void expression_destroy(Expression *expression) {
    switch (expression->type) {
        case EXPRESSION_NONE:
        case EXPRESSION_LITERAL_NIL:
            break;

        case EXPRESSION_PRIORITY:
        {
            PriorityExpression *priorityExpression = expression->value;

            expression_destroy(&priorityExpression->value);
            
            free(priorityExpression);
            
            break;    
        }
        
        case EXPRESSION_NOT:
        {
            NotExpression *notExpression = expression->value;

            expression_destroy(&notExpression->value);
            
            free(notExpression);
            
            break;
        }
            
        case EXPRESSION_BIT_NOT:
        {
            BitNotExpression *bitNotExpression = expression->value;

            expression_destroy(&bitNotExpression->value);
            
            free(bitNotExpression);

            break;
        }
            
        case EXPRESSION_COUNT:
        {
            CountExpression *countExpression = expression->value;
            
            expression_destroy(&countExpression->expression);

            free(countExpression);
            
            break;
        }

        case EXPRESSION_LITERAL_BOOLEAN:
        {
            LiteralBooleanExpression *literalBooleanExpression = expression->value;
            
            free(literalBooleanExpression);
            
            break;    
        }
        case EXPRESSION_LITERAL_NUMBER:
        {
            LiteralNumberExpression *literalNumberExpression = expression->value;

            free(literalNumberExpression);
            
            break;    
        }
        case EXPRESSION_LITERAL_STRING:
        {
            LiteralStringExpression *literalStringExpression = expression->value;

            free(literalStringExpression);
            
            break;
        }
            
        case EXPRESSION_BINARY:
        {
            BinaryExpression *binaryExpression = expression->value;

            expression_destroy(&binaryExpression->left);
            expression_destroy(&binaryExpression->right);
            
            free(binaryExpression);
            
            break;
        }

        case EXPRESSION_VARIABLE: {
            VariableExpression *variableExpression = expression->value;
            free(variableExpression);

            break;
        }
        case EXPRESSION_VARIABLE_NAME_INDEX: {
            VariableNameIndexExpression *variableNameIndexExpression = expression->value;
            
            expression_destroy(&variableNameIndexExpression->first);

            free(variableNameIndexExpression);

            break;
        }
        case EXPRESSION_VARIABLE_INDEX:
        {
            VariableIndexExpression *variableIndexExpression = expression->value;

            expression_destroy(&variableIndexExpression->first);
            expression_destroy(&variableIndexExpression->index);
            
            free(variableIndexExpression);
            
            break;
        }
        case EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF: {
            VariableNameIndexWithSelfExpression *variableNameIndexWithSelfExpression = expression->value;

            expression_destroy(&variableNameIndexWithSelfExpression->first);

            free(variableNameIndexWithSelfExpression);

            break;
        }
        
        case EXPRESSION_FUNCTION: {
            FunctionExpression *functionExpression = expression->value;
            
            if (functionExpression->parameters != NULL) {
                FunctionParameterNode *node = functionExpression->parameters;
                while (node != NULL) {
                    {
                        FunctionParameter *functionParameter = node->value;
                        free(functionParameter);
                    };
                    FunctionParameterNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            if (functionExpression->statements != NULL) {
                StatementNode *node = functionExpression->statements;
                while (node != NULL) {
                    {
                        statement_destroy(&node->value);
                    };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            free(functionExpression);

            break;
        }
        case EXPRESSION_FUNCTION_CALL: {
            FunctionCallExpression *functionCallExpression = expression->value;

            expression_destroy(&functionCallExpression->index);
            if (functionCallExpression->arguments != NULL) {
                ExpressionNode *node = functionCallExpression->arguments;
                while (node != NULL) {
                    {
                        expression_destroy(node->value);
                        free(node->value);
                    };
                    ExpressionNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            free(functionCallExpression);

            break;
        }
        
        case EXPRESSION_TABLE: {
            TableExpression *tableExpression = expression->value;

            TableInitializerExpressionNode *node = tableExpression->initializers;
            while (node != NULL) {
                {
                    switch (node->type) {
                        case TABLE_INITIALIZER_NONE:
                            break;
                        
                        case TABLE_INITIALIZER_NAMED:
                        {
                            TableNamedInitializerExpression *tableNamedInitializerExpression = node->value;
                            
                            expression_destroy(&tableNamedInitializerExpression->initializer);
                            
                            free(tableNamedInitializerExpression);
                            
                            break;
                        }
                        
                        case TABLE_INITIALIZER_INDEX:
                        {
                            TableIndexInitializerExpression *tableIndexInitializerExpression = node->value;
                            
                            expression_destroy(&tableIndexInitializerExpression->index);
                            expression_destroy(&tableIndexInitializerExpression->initializer);

                            free(tableIndexInitializerExpression);
                            
                            break;
                        }
                            
                        case TABLE_INITIALIZER_EXPRESSION:
                        {
                            expression_destroy(node->value);
                            free(node->value);
                            break;
                        }
                    }
                };
                TableInitializerExpressionNode *nextNode = node->next;
                free(node);
                node = nextNode;
            }

            free(tableExpression);

            break;
        }

        default:
            UNIMPLEMENTED("expression_destroy")
    }

    //NOTE: could be removed since it is not needed
    expression->type = EXPRESSION_NONE;
    expression->position = (Position) {0};
    expression->value = NULL;
}
