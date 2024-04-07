#include "statement.h"

#include <string.h>

#include "expression_structs.h"
#include "utils.h"
#include "output.h"

Statement local_statement_parse(Parser *p) {
    if (token_is_keyword("function", &p->cur_token)) {
        Token func = p->cur_token;
        parser_consume(p);

        Token funcName = p->cur_token;
        if (funcName.type != TOKEN_SYMBOL) {
            const char *message = "missing function name";
            output_miss_symbol(p, &func.position, message, strlen(message));
        } else {
            parser_consume(p);
        }

        Expression indexExpression = {0};

        SAFE_MALLOC(VariableExpression, variableExpression)
        variableExpression->symbol = funcName;

        indexExpression.type = EXPRESSION_VARIABLE;
        indexExpression.position = variableExpression->symbol.position;
        indexExpression.value = variableExpression;

        Statement statement = {0};

        SAFE_MALLOC(FunctionDeclaration, functionDeclaration)
        functionDeclaration->expression = function_expression_parse(p, &func);
        functionDeclaration->index = indexExpression;

        statement.type = STATEMENT_FUNCTION_DECLARATION;
        statement.value = functionDeclaration;
        statement.position = position_from_to(&func.position, &functionDeclaration->expression.position);
        return statement;
    }

    if (p->cur_token.type == TOKEN_SYMBOL) {
        Token symbol = p->cur_token;
        parser_consume(p);

        Token equal = p->cur_token;
        if (equal.type != TOKEN_EQUAL) {
            Statement statement = {0};

            SAFE_MALLOC(VariableDeclaration, variableDeclaration)
            variableDeclaration->initializer = NULL;
            variableDeclaration->symbol = symbol;

            statement.position = symbol.position;
            statement.type = STATEMENT_VARIABLE_DECLARATION;
            statement.value = variableDeclaration;
            return statement;
        } else {
            parser_consume(p);
        }

        SAFE_MALLOC(Expression, initializer);
        {
            Expression initializerStack = expression_parse(p);
            if (initializerStack.type == EXPRESSION_NONE) {
                const char *message = "missing expression as initializer for variable declaration";
                output_miss_expression(p, &equal.position, message, strlen(message));
            }
            memcpy(initializer, &initializerStack, sizeof(Expression));
        }

        Statement statement = {0};

        SAFE_MALLOC(VariableDeclaration, variableDeclaration)
        variableDeclaration->initializer = initializer;
        variableDeclaration->symbol = symbol;

        statement.position = position_from_to(&symbol.position, &initializer->position);
        statement.type = STATEMENT_VARIABLE_DECLARATION;
        statement.value = variableDeclaration;
        return statement;
    }

    return (Statement) {0};
}

bool is_end_of_if_body(Token *token) {
    return token_is_keyword("end", token) || token_is_keyword("elseif", token) || token_is_keyword("else", token);
}

Statement if_statement_parse(Parser *p, Token *_if) {
    Expression condition = expression_parse(p);
    if (condition.type == EXPRESSION_NONE) {
        const char *message = "missing if condition";
        output_miss_expression(p, &_if->position, message, strlen(message));
    }

    if (!token_is_keyword("then", &p->cur_token)) {
        const char *message = "missing \"then\" after condition";
        Position pos = position_after(&condition.position);
        output_miss_keyword_then(p, &pos, message, strlen(message));
    } else {
        parser_consume(p);
    }

    StatementNode *ifBodyStatements = statement_parse_body(p, is_end_of_if_body);

    SAFE_MALLOC(ElseIfStatementNode, elseIfBodyStatementsHead)
    elseIfBodyStatementsHead->next = NULL;
    elseIfBodyStatementsHead->value = NULL;
    SAFE_MALLOC(ElseStatementNode, elseBodyStatementsHead)
    elseBodyStatementsHead->next = NULL;
    elseBodyStatementsHead->value = NULL;
    {
        ElseIfStatementNode *elseIfBodyStatements = elseIfBodyStatementsHead;
        ElseStatementNode *elseBodyStatements = elseBodyStatementsHead;

        while (true) {
            Token elseToken = p->cur_token;
            if (token_is_keyword("elseif", &elseToken)) {
                parser_consume(p);

                Expression elseIfCondition = expression_parse(p);
                if (elseIfCondition.type == EXPRESSION_NONE) {
                    const char *message = "missing condition for elseif";
                    output_miss_expression(p, &elseToken.position, message, strlen(message));
                }

                if (!token_is_keyword("then", &p->cur_token)) {
                    const char *message = "missing \"then\" keyword after elseif condition";
                    output_miss_expression(p, &elseToken.position, message, strlen(message));
                } else {
                    parser_consume(p);
                }

                SAFE_MALLOC(ElseIfStatement, elseIfStatement)
                elseIfStatement->statements = statement_parse_body(p, is_end_of_if_body);
                elseIfStatement->condition = elseIfCondition;

                SAFE_MALLOC(ElseIfStatementNode, node)
                node->value = elseIfStatement;
                node->next = NULL;

                elseIfBodyStatements->next = node;
                elseIfBodyStatements = node;
                continue;
            }

            if (token_is_keyword("else", &elseToken)) {
                parser_consume(p);

                SAFE_MALLOC(ElseStatement, elseStatement)
                elseStatement->statements = statement_parse_body(p, is_end_of_if_body);

                SAFE_MALLOC(ElseStatementNode, node)
                node->value = elseStatement;
                node->next = NULL;

                elseBodyStatements->next = node;
                elseBodyStatements = node;
                continue;
            }

            break;
        }
    }

    Token end = p->cur_token;
    if (!token_is_keyword("end", &p->cur_token)) {
        const char *message = "missing \"end\" of if statement";
        output_miss_keyword_end(p, &_if->position, message, strlen(message));
    } else {
        parser_consume(p);
    }

    Statement statement = {0};

    SAFE_MALLOC(IfStatement, _ifStatement)
    _ifStatement->condition = condition;
    _ifStatement->if_body = ifBodyStatements;
    _ifStatement->else_if_body_nodes = elseIfBodyStatementsHead->next;
    free(elseIfBodyStatementsHead);
    _ifStatement->else_body_nodes = elseBodyStatementsHead->next;
    free(elseBodyStatementsHead);

    statement.type = STATEMENT_IF;
    statement.value = _ifStatement;
    statement.position = position_from_to(&_if->position, &end.position);
    return statement;
}

bool is_end_of_for_body(Token *token) {
    return token_is_keyword("end", token);
}

Statement for_generic_statement_parse(Parser *p, Token *_for, Token symbol, Position *lastPos) {
    SAFE_MALLOC(TokenNode, symbolsHead)
    symbolsHead->value = symbol;

    TokenNode *symbolsCurrent = symbolsHead;
    while (p->cur_token.type == TOKEN_COMMA) {
        Token comma = p->cur_token;
        parser_consume(p);

        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol";
            output_miss_symbol(p, &comma.position, message, strlen(message));

            continue;
        }

        Token symbolValue = p->cur_token;
        parser_consume(p);

        SAFE_MALLOC(TokenNode, node)

        node->next = NULL;
        node->value = symbolValue;

        symbolsCurrent->next = node;
        symbolsCurrent = node;
    }
    lastPos = &symbolsCurrent->value.position;

    if (!token_is_keyword("in", &p->cur_token)) {
        const char *message = "missing \"in\" keyword for generic for loop";
        output_miss_keyword_in(p, lastPos, message, strlen(message));
    } else {
        lastPos = &p->cur_token.position;
        parser_consume(p);
    }

    Expression getIterator = expression_parse(p);
    if (getIterator.type == EXPRESSION_NONE) {
        const char *message = "missing get iterator expression";
        output_miss_expression(p, lastPos, message, strlen(message));
    } else {
        lastPos = &getIterator.position;
    }

    if (!token_is_keyword("do", &p->cur_token)) {
        const char *message = "missing \"do\" keyword for generic for loop";
        output_miss_keyword_do(p, lastPos, message, strlen(message));
    } else {
        lastPos = &p->cur_token.position;
        parser_consume(p);
    }

    StatementNode *statements = statement_parse_body(p, is_end_of_for_body);
    for (StatementNode *node = statements; node != NULL; node = node->next) {
        if (node->next != NULL) {
            continue;
        }
        lastPos = &node->value.position;
    }

    if (!token_is_keyword("end", &p->cur_token)) {
        const char *message = "missing \"end\" keyword to signal end of generic for loop";
        output_miss_keyword_end(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &p->cur_token.position;
        parser_consume(p);
    }

    Statement statement = {0};

    SAFE_MALLOC(ForGenericLoopStatement, forGenericLoopStatement)

    forGenericLoopStatement->symbols = symbolsHead;
    forGenericLoopStatement->get_iterator = getIterator;
    forGenericLoopStatement->statements = statements;

    statement.type = STATEMENT_FOR_GENERIC;
    statement.value = forGenericLoopStatement;
    statement.position = position_from_to(&_for->position, lastPos);

    return statement;
}

Statement for_statement_parse(Parser *p, Token *_for) {
    Position *lastPos = {0};
    Token symbol;
    if (p->cur_token.type != TOKEN_SYMBOL) {
        const char *message = "missing symbol for iterator";
        output_miss_symbol(p, &_for->position, message, strlen(message));
    } else {
        symbol = p->cur_token;
        lastPos = &symbol.position;
        parser_consume(p);
    }

    if (p->cur_token.type != TOKEN_EQUAL) {
        return for_generic_statement_parse(p, _for, symbol, lastPos);
    } else {
        lastPos = &p->cur_token.position;
        parser_consume(p);
    }

    Expression iteratorInitializer = expression_parse(p);
    if (iteratorInitializer.type == EXPRESSION_NONE) {
        const char *message = "missing iterator initializer";
        output_miss_expression(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &iteratorInitializer.position;
    }

    Token comma_iterator_condition = p->cur_token;
    if (comma_iterator_condition.type != TOKEN_COMMA) {
        const char *message = "missing comma for iterator and condition separation";
        output_miss_comma(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &comma_iterator_condition.position;
        parser_consume(p);
    }

    Expression condition = expression_parse(p);
    if (condition.type == EXPRESSION_NONE) {
        const char *message = "missing condition";
        output_miss_expression(p, &_for->position, message, strlen(message));
    }

    Token comma_condition_increment = p->cur_token;
    Expression increment = expression_empty();
    if (comma_condition_increment.type == TOKEN_COMMA) {
        lastPos = &comma_condition_increment.position;
        parser_consume(p);

        increment = expression_parse(p);
    } else {
        SAFE_MALLOC(LiteralNumberExpression, literalNumberExpression)
        literalNumberExpression->value = 1;

        increment.type = EXPRESSION_LITERAL_NUMBER;
        increment.value = literalNumberExpression;
    }

    Token _do = p->cur_token;
    if (!token_is_keyword("do", &_do)) {
        const char *message = "missing \"do\" keyword for numeric for loop";
        output_miss_keyword_do(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &_do.position;
        parser_consume(p);
    }

    StatementNode *statements = statement_parse_body(p, is_end_of_for_body);
    for (StatementNode *node = statements; node != NULL; node = node->next) {
        if (node->next != NULL) {
            continue;
        }
        lastPos = &node->value.position;
    }

    Token end = p->cur_token;
    if (!token_is_keyword("end", &end)) {
        const char *message = "missing \"end\" keyword for numeric for loop";
        output_miss_keyword_end(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &end.position;
        parser_consume(p);
    }

    Statement statement = {0};

    SAFE_MALLOC(ForNumericLoopStatement, forNumericLoopStatement)

    forNumericLoopStatement->symbol = symbol;
    forNumericLoopStatement->initializer = iteratorInitializer;
    forNumericLoopStatement->condition = condition;
    forNumericLoopStatement->increment = increment;
    forNumericLoopStatement->statements = statements;

    statement.type = STATEMENT_FOR_NUMERIC;
    statement.value = forNumericLoopStatement;
    statement.position = position_from_to(&_for->position, lastPos);
    return statement;
}

bool is_end_of_do_body(Token *token) {
    return token_is_keyword("end", token);
}

bool is_end_of_repeat_body(Token *token) {
    return token_is_keyword("until", token);
}

bool is_end_of_while_body(Token *token) {
    return token_is_keyword("end", token);
}

Statement statement_parse(Parser *p) {
    Token token = p->cur_token;
    if (token.type == TOKEN_SYMBOL) {
        Expression index;
        {
            Token symbol = p->cur_token;
            parser_consume(p);

            SAFE_MALLOC(VariableExpression, variableExpression)
            variableExpression->symbol = symbol;

            index.type = EXPRESSION_VARIABLE;
            index.position = symbol.position;
            index.value = variableExpression;
        }

        while (true) {
            if (p->cur_token.type == TOKEN_DOT) {
                Token dot = p->cur_token;
                parser_consume(p);
                Position *lastPos = &dot.position;

                Token symbol;
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \".\"";
                    output_miss_symbol(p, &dot.position, message, strlen(message));
                } else {
                    symbol = p->cur_token;
                    parser_consume(p);
                    lastPos = &symbol.position;
                }

                Expression valueExpression;

                SAFE_MALLOC(VariableNameIndexExpression, variableNameIndexExpression)
                variableNameIndexExpression->first = index;
                variableNameIndexExpression->index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX;
                valueExpression.position = position_from_to(&index.position, lastPos);
                valueExpression.value = variableNameIndexExpression;

                index = valueExpression;
                continue;
            }

            break;
        }

        Token nextToken = p->cur_token;
        if (nextToken.type == TOKEN_EQUAL) {
            parser_consume(p);

            Position *lastPos;

            Expression exp = expression_parse(p);
            if (exp.type == EXPRESSION_NONE) {
                const char *message = "missing expression after equal \"=\" operator";
                output_miss_expression(p, &nextToken.position, message, strlen(message));

                lastPos = &nextToken.position;
            } else {
                lastPos = &exp.position;
            }

            Statement statement = {0};

            SAFE_MALLOC(AssignmentStatement, variableAssignmentStatement)
            variableAssignmentStatement->expression = exp;
            variableAssignmentStatement->index = index;

            statement.type = STATEMENT_ASSIGNMENT;
            statement.value = variableAssignmentStatement;
            statement.position = position_from_to(&token.position, lastPos);
            return statement;
        }

        SAFE_MALLOC(Expression, expression);
        {
            Expression expressionStack = expression_chain_parse(p, index);
            if (expressionStack.type != EXPRESSION_FUNCTION_CALL) {
                const char *message = "unexpected expression as statement";
                output_unexpected_expression(p, &expressionStack.position, message, strlen(message));
            }
            memcpy(expression, &expressionStack, sizeof(Expression));
        }
        Statement statement = {0};

        statement.type = STATEMENT_EXPRESSION;
        statement.value = expression;
        statement.position = expression->position;
        return statement;
    }

    if (token.type == TOKEN_GOTO) {
        parser_consume(p);
        Position *lastPos = &token.position;

        SAFE_MALLOC(Token, symbol)
        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol after goto point operator";
            output_miss_symbol(p, &token.position, message, strlen(message));
        } else {
            memcpy(symbol, &p->cur_token, sizeof(Token));
            lastPos = &symbol->position;
            parser_consume(p);
        }

        if (p->cur_token.type != TOKEN_GOTO) {
            const char *message = "missing goto operator \"::\" after goto point symbol";
            output_miss_goto_point(p, lastPos, message, strlen(message));
        } else {
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }

        Statement statement = {0};

        statement.type = STATEMENT_GOTO_POINT;
        statement.position = position_from_to(&token.position, lastPos);
        statement.value = symbol;
        return statement;
    }

    if (token_is_keyword("local", &token)) {
        parser_consume(p);

        Statement child = local_statement_parse(p);
        if (child.type == STATEMENT_NONE) {
            output_unexpected_token(p, &token, "missing variable or function declaration after: %s");
            return (Statement) {0};
        }

        SAFE_MALLOC(LocalStatement, localStatement)
        localStatement->child = child;

        Statement statement = {0};
        statement.type = STATEMENT_LOCAL;
        statement.value = localStatement;
        statement.position = position_from_to(&token.position, &child.position);
        return statement;
    }

    if (token_is_keyword("function", &token)) {
        parser_consume(p);

        Expression index;
        {
            Token symbol = {0};
            if (p->cur_token.type != TOKEN_SYMBOL) {
                const char *message = "missing function name";
                output_miss_symbol(p, &token.position, message, strlen(message));
            } else {
                symbol = p->cur_token;
                parser_consume(p);
            }

            SAFE_MALLOC(VariableExpression, variableExpression)
            variableExpression->symbol = symbol;

            index.type = EXPRESSION_VARIABLE;
            index.value = variableExpression;

            if (symbol.position.start_column == 0) {
                index.position = (Position){0};
            } else {
                index.position = symbol.position;
            }
        }

        while (true) {
            if (p->cur_token.type == TOKEN_DOT) {
                Token dot = p->cur_token;
                parser_consume(p);
                Position *lastPos = &dot.position;

                Token symbol = {0};
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \".\"";
                    output_miss_symbol(p, &dot.position, message, strlen(message));
                } else {
                    symbol = p->cur_token;
                    lastPos = &symbol.position;
                    parser_consume(p);
                }

                Expression valueExpression;

                SAFE_MALLOC(VariableNameIndexExpression, variableNameIndexExpression)
                variableNameIndexExpression->first = index;
                variableNameIndexExpression->index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX;
                valueExpression.position = position_from_to(&index.position, lastPos);
                valueExpression.value = variableNameIndexExpression;

                index = valueExpression;
                continue;
            }

            if (p->cur_token.type == TOKEN_COLON) {
                Token colon = p->cur_token;
                parser_consume(p);
                Position *lastPos = &colon.position;

                Token symbol = {0};
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \":\"";
                    output_miss_symbol(p, &colon.position, message, strlen(message));
                } else {
                    symbol = p->cur_token;
                    parser_consume(p);
                    lastPos = &symbol.position;
                }

                Expression valueExpression;

                SAFE_MALLOC(VariableNameIndexWithSelfExpression, variableNameIndexWithSelfExpression)
                variableNameIndexWithSelfExpression->first = index;
                variableNameIndexWithSelfExpression->index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF;
                valueExpression.position = position_from_to(&index.position, lastPos);
                valueExpression.value = variableNameIndexWithSelfExpression;

                index = valueExpression;
            }

            break;
        }

        Statement statement = {0};

        SAFE_MALLOC(FunctionDeclaration, functionDeclaration)
        functionDeclaration->expression = function_expression_parse(p, &token);
        functionDeclaration->index = index;

        statement.type = STATEMENT_FUNCTION_DECLARATION;
        statement.value = functionDeclaration;
        statement.position = position_from_to(&token.position, &functionDeclaration->expression.position);
        return statement;
    }

    if (token_is_keyword("if", &token)) {
        parser_consume(p);

        return if_statement_parse(p, &token);
    }

    if (token_is_keyword("for", &token)) {
        parser_consume(p);

        return for_statement_parse(p, &token);
    }

    if (token_is_keyword("return", &token)) {
        parser_consume(p);

        Expression expression = expression_parse(p);

        Statement statement = {0};

        SAFE_MALLOC(ReturnStatement, returnStatement)
        returnStatement->expression = expression;

        statement.type = STATEMENT_RETURN;
        statement.value = returnStatement;
        statement.position = position_from_to(&token.position, &expression.position);
        return statement;
    }

    if (token_is_keyword("do", &token)) {
        parser_consume(p);
        Position *lastPos = &token.position;

        StatementNode *statements = statement_parse_body(p, is_end_of_do_body);
        for (StatementNode *node = statements; node != NULL; node = node->next) {
            if (node->next != NULL) {
                continue;
            }
            lastPos = &node->value.position;
        }

        Token end = p->cur_token;
        if (!token_is_keyword("end", &end)) {
            const char *message = "missing \"end\" keyword to signal end of do block";
            output_miss_keyword_end(p, &token.position, message, strlen(message));
        } else {
            lastPos = &end.position;
            parser_consume(p);
        }

        Statement statement = {0};

        SAFE_MALLOC(DoStatement, doStatement)

        doStatement->statements = statements;

        statement.type = STATEMENT_DO;
        statement.position = position_from_to(&token.position, lastPos);
        statement.value = doStatement;
        return statement;
    }

    if (token_is_keyword("repeat", &token)) {
        parser_consume(p);
        Position *lastPos = &token.position;

        StatementNode *statements = statement_parse_body(p, is_end_of_repeat_body);
        for (StatementNode *node = statements; node != NULL; node = node->next) {
            if (node->next != NULL) {
                continue;
            }
            lastPos = &node->value.position;
        }

        Token until = p->cur_token;
        if (!token_is_keyword("until", &until)) {
            const char *message = "missing \"until\" keyword to signal until of repeat loop";
            output_miss_keyword_end(p, &token.position, message, strlen(message));
        } else {
            lastPos = &until.position;
            parser_consume(p);
        }

        Expression condition = expression_parse(p);
        if (condition.type == EXPRESSION_NONE) {
            const char *message = "missing condition for repeat loop";
            output_miss_expression(p, lastPos, message, strlen(message));
        } else {
            lastPos = &condition.position;
            parser_consume(p);
        }

        Statement statement = {0};

        SAFE_MALLOC(RepeatStatement, repeatStatement)

        repeatStatement->statements = statements;
        repeatStatement->condition = condition;

        statement.type = STATEMENT_REPEAT;
        statement.position = position_from_to(&token.position, lastPos);
        statement.value = repeatStatement;
        return statement;
    }

    if (token_is_keyword("while", &token)) {
        parser_consume(p);
        Position *lastPos = &token.position;

        Expression condition = expression_parse(p);
        if (condition.type == EXPRESSION_NONE) {
            const char *message = "missing condition for while loop";
            output_miss_expression(p, lastPos, message, strlen(message));
        }

        if (!token_is_keyword("do", &p->cur_token)) {
            const char *message = "missing \"do\" keyword after while loop condition";
            output_miss_expression(p, lastPos, message, strlen(message));
        } else {
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }

        StatementNode *statements = statement_parse_body(p, is_end_of_while_body);
        for (StatementNode *node = statements; node != NULL; node = node->next) {
            if (node->next != NULL) {
                continue;
            }
            lastPos = &node->value.position;
        }

        if (!token_is_keyword("end", &p->cur_token)) {
            const char *message = "missing \"end\" keyword to signal end of while loop";
            output_miss_keyword_end(p, lastPos, message, strlen(message));
        } else {
            lastPos = &p->cur_token.position;
            parser_consume(p);
        }

        Statement statement = {0};

        SAFE_MALLOC(WhileStatement, whileStatement)

        whileStatement->condition = condition;
        whileStatement->statements = statements;

        statement.type = STATEMENT_WHILE;
        statement.position = position_from_to(&token.position, lastPos);
        statement.value = whileStatement;
        return statement;
    }

    if (token_is_keyword("break", &token)) {
        parser_consume(p);

        Statement statement = {0};

        statement.type = STATEMENT_BREAK;
        statement.position = token.position;
        return statement;
    }

    if (token_is_keyword("goto", &token)) {
        parser_consume(p);
        Position *lastPos = &token.position;

        SAFE_MALLOC(Token, symbol)
        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol after \"goto\" symbol";
            output_miss_symbol(p, lastPos, message, strlen(message));
        } else {
            memcpy(symbol, &p->cur_token, sizeof(Token));
            lastPos = &symbol->position;
            parser_consume(p);
        }

        Statement statement = {0};

        statement.type = STATEMENT_GOTO;
        statement.position = position_from_to(&token.position, lastPos);
        statement.value = symbol;
        return statement;
    }

    if (token.type == TOKEN_END) {
        Statement statement = {0};
        statement.type = STATEMENT_END;
        return statement;
    }

    output_unexpected_token(p, &token, "%s");
    parser_consume(p);

    return (Statement) {0};
}

StatementNode *statement_parse_body(Parser *p, bool (*isEnd)(Token *token)) {
    SAFE_MALLOC(StatementNode, statementHead)
    statementHead->value = (Statement) {0};
    StatementNode *statementCurrent = statementHead;

    while (true) {
        if (isEnd(&p->cur_token) || p->cur_token.type == TOKEN_END) {
            break;
        }

        Statement statement = statement_parse(p);
        if (statement.type != STATEMENT_NONE) {
            SAFE_MALLOC(StatementNode, node)

            node->value = statement;
            node->next = NULL;

            statementCurrent->next = node;
            statementCurrent = node;
        }
    }

    statementCurrent = statementHead->next;
    free(statementHead);
    return statementCurrent;
}

void statement_destroy(Statement *statement) {

    switch (statement->type) {
        case STATEMENT_NONE:
        case STATEMENT_END:
            break;
            
        case STATEMENT_DO: {
            DoStatement *doStatement = statement->value;

            if (doStatement->statements != NULL) {
                StatementNode *node = doStatement->statements;
                while (node != NULL) {
                    { statement_destroy(&node->value); };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            };

            free(doStatement);

            break;
        }

        case STATEMENT_LOCAL: {
            LocalStatement *localStatement = statement->value;

            statement_destroy(&localStatement->child);

            free(localStatement);

            break;
        }

        case STATEMENT_VARIABLE_DECLARATION: {
            VariableDeclaration *variableDeclaration = statement->value;

            if (variableDeclaration->initializer != NULL) {
                expression_destroy(variableDeclaration->initializer);
                free(variableDeclaration->initializer);
            }
            
            free(variableDeclaration);

            break;
        }
        case STATEMENT_FUNCTION_DECLARATION: {
            FunctionDeclaration *functionDeclaration = statement->value;

            expression_destroy(&functionDeclaration->expression);
            expression_destroy(&functionDeclaration->index);

            free(functionDeclaration);

            break;
        }

        case STATEMENT_RETURN: {
            ReturnStatement *returnStatement = statement->value;

            expression_destroy(&returnStatement->expression);

            free(returnStatement);

            break;
        }

        case STATEMENT_ASSIGNMENT: {
            AssignmentStatement *assignmentStatement = statement->value;

            expression_destroy(&assignmentStatement->expression);
            expression_destroy(&assignmentStatement->index);

            free(assignmentStatement);

            break;
        }

        case STATEMENT_IF: {
            IfStatement *ifStatement = statement->value;

            if (ifStatement->if_body != NULL) {
                StatementNode *node = ifStatement->if_body;
                while (node != NULL) {
                    {
                        statement_destroy(&node->value);
                    };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            if (ifStatement->else_if_body_nodes != NULL) {
                ElseIfStatementNode *node = ifStatement->else_if_body_nodes;
                while (node != NULL) {
                    {
                        ElseIfStatement *elseIfStatement = node->value;
                        if (elseIfStatement->statements != NULL) {
                            StatementNode *node2 = elseIfStatement->statements;
                            while (node2 != NULL) {
                                { statement_destroy(&node2->value); };
                                StatementNode *nextNode = node2->next;
                                free(node2);
                                node2 = nextNode;
                            }
                        }
                        
                        expression_destroy(&elseIfStatement->condition);
                        free(elseIfStatement);
                    };
                    ElseIfStatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            if (ifStatement->else_body_nodes != NULL) {
                ElseStatementNode *node = ifStatement->else_body_nodes;
                while (node != NULL) {
                    {
                        ElseStatement *elseStatement = node->value;
                        if (elseStatement->statements != NULL) {
                            StatementNode *node2 = elseStatement->statements;
                            while (node2 != NULL) {
                                { statement_destroy(&node2->value); };
                                StatementNode *nextNode = node2->next;
                                free(node2);
                                node2 = nextNode;
                            }
                        }
                        free(elseStatement);
                    };
                    ElseStatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            expression_destroy(&ifStatement->condition);
            
            free(ifStatement);

            break;
        }


        case STATEMENT_EXPRESSION:
            expression_destroy(statement->value);
            free(statement->value);
            break;

        case STATEMENT_GOTO_POINT:
        case STATEMENT_GOTO:
            // freeing symbol reference
            free(statement->value);
            break;
            
        case STATEMENT_BREAK:
            break;

        case STATEMENT_FOR_NUMERIC: {
            ForNumericLoopStatement *forNumericLoopStatement = statement->value;

            if (forNumericLoopStatement->statements != NULL) {
                StatementNode *node = forNumericLoopStatement->statements;
                while (node != NULL) {
                    { statement_destroy(&node->value); };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            expression_destroy(&forNumericLoopStatement->initializer);
            expression_destroy(&forNumericLoopStatement->condition);
            expression_destroy(&forNumericLoopStatement->increment);
            
            free(forNumericLoopStatement);

            break;
        }

        case STATEMENT_FOR_GENERIC: {
            ForGenericLoopStatement *forGenericLoopStatement = statement->value;

            if (forGenericLoopStatement->statements != NULL) {
                StatementNode *node = forGenericLoopStatement->statements;
                while (node != NULL) {
                    { statement_destroy(&node->value); };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            if (forGenericLoopStatement->symbols != NULL) {
                TokenNode *node = forGenericLoopStatement->symbols;
                while (node != NULL) {
                    TokenNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            };

            expression_destroy(&forGenericLoopStatement->get_iterator);
            
            free(forGenericLoopStatement);

            break;
        }

        case STATEMENT_REPEAT: {
            RepeatStatement *repeatStatement = statement->value;

            if (repeatStatement->statements != NULL) {
                StatementNode *node = repeatStatement->statements;
                while (node != NULL) {
                    { statement_destroy(&node->value); };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            };

            expression_destroy(&repeatStatement->condition);
            
            free(repeatStatement);

            break;
        }

        case STATEMENT_WHILE: {
            WhileStatement *whileStatement = statement->value;

            if (whileStatement->statements != NULL) {
                StatementNode *node = whileStatement->statements;
                while (node != NULL) {
                    { statement_destroy(&node->value); };
                    StatementNode *nextNode = node->next;
                    free(node);
                    node = nextNode;
                }
            }

            expression_destroy(&whileStatement->condition);
            
            free(whileStatement);

            break;
        }
    }

    //NOTE: could be removed since it is not needed
    statement->type = STATEMENT_NONE;
    statement->position = (Position) {0};
    statement->value = NULL;
}
