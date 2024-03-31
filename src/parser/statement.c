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

        Expression indexExpression;
        VariableExpression varExpression;
        varExpression.symbol = symbol_from_token(&funcName);
        indexExpression.type = EXPRESSION_VARIABLE;
        indexExpression.position = varExpression.symbol->position;
        indexExpression.value = &varExpression;

        Statement statement = {0};

        FunctionDeclaration functionDeclaration;
        functionDeclaration.expression = function_expression_parse(p, &func);
        functionDeclaration.index = indexExpression;

        statement.type = STATEMENT_FUNCTION_DECLARATION;
        statement.value = &functionDeclaration;
        statement.position = position_from_to(&func.position, &functionDeclaration.expression.position);
        return statement;
    }

    if (p->cur_token.type == TOKEN_SYMBOL) {
        Token symbol = p->cur_token;
        parser_consume(p);

        Token equal = p->cur_token;
        if (equal.type != TOKEN_EQUAL) {
            Statement statement = {0};

            VariableDeclaration variableDeclaration;
            variableDeclaration.initializer = NULL;
            variableDeclaration.symbol = symbol_from_token(&symbol);

            statement.position = symbol.position;
            statement.type = STATEMENT_VARIABLE_DECLARATION;
            statement.value = &variableDeclaration;
            return statement;
        } else {
            parser_consume(p);
        }

        Expression initializer = expression_parse(p);
        if (initializer.type == EXPRESSION_NONE) {
            const char *message = "missing expression as initializer for variable declaration";
            output_miss_expression(p, &equal.position, message, strlen(message));
        }

        Statement statement = {0};

        VariableDeclaration variableDeclaration;
        variableDeclaration.initializer = &initializer;
        variableDeclaration.symbol = symbol_from_token(&symbol);

        statement.position = position_from_to(&symbol.position, &initializer.position);
        statement.type = STATEMENT_VARIABLE_DECLARATION;
        statement.value = &variableDeclaration;
        return statement;
    }

    return (Statement){0};
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

    ElseIfStatementNode *elseIfBodyStatementsHead = malloc(sizeof(ElseIfStatementNode));
    if (elseIfBodyStatementsHead == NULL) {
        UNIMPLEMENTED("statement_parse");
    }

    elseIfBodyStatementsHead->next = NULL;
    elseIfBodyStatementsHead->value = NULL;
    ElseStatementNode *elseBodyStatementsHead = malloc(sizeof(ElseStatementNode));
    if (elseBodyStatementsHead == NULL) {
        UNIMPLEMENTED("statement_parse");
    }

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

                ElseIfStatement *elseIfStatement = malloc(sizeof(ElseIfStatement));
                if (elseIfStatement == NULL) {
                    UNIMPLEMENTED("statement_parse");
                }

                elseIfStatement->statements = statement_parse_body(p, is_end_of_if_body);
                elseIfStatement->condition = elseIfCondition;

                ElseIfStatementNode *node = malloc(sizeof(ElseIfStatementNode));
                if (node == NULL) {
                    UNIMPLEMENTED("statement_parse");
                }

                node->value = elseIfStatement;
                node->next = NULL;

                elseIfBodyStatements->next = node;
                elseIfBodyStatements = node;
                continue;
            }

            if (token_is_keyword("else", &elseToken)) {
                parser_consume(p);

                ElseStatement *elseStatement = malloc(sizeof(ElseStatement));
                if (elseStatement == NULL) {
                    UNIMPLEMENTED("statement_parse");
                }

                elseStatement->statements = statement_parse_body(p, is_end_of_if_body);

                ElseStatementNode *node = malloc(sizeof(ElseStatementNode));
                if (node == NULL) {
                    UNIMPLEMENTED("statement_parse");
                }

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

    IfStatement _ifStatement;
    _ifStatement.condition = condition;
    _ifStatement.if_body = ifBodyStatements;
    _ifStatement.else_if_body_nodes = elseIfBodyStatementsHead->next;
    free(elseIfBodyStatementsHead);
    _ifStatement.else_body_nodes = elseBodyStatementsHead->next;
    free(elseBodyStatementsHead);

    statement.type = STATEMENT_IF;
    statement.value = &_ifStatement;
    statement.position = position_from_to(&_if->position, &end.position);
    return statement;
}

bool is_end_of_for_body(Token *token) {
    return token_is_keyword("end", token);
}

Statement for_generic_statement_parse(Parser *p, Token *_for, Symbol *symbol, Position *lastPos) {
    SymbolNode *symbolsHead = malloc(sizeof(SymbolNode));
    if (symbolsHead == NULL) {
        UNIMPLEMENTED("for_generic_statement_parse");
    }
    symbolsHead->value = symbol;

    SymbolNode *symbolsCurrent = symbolsHead;
    while (p->cur_token.type == TOKEN_COMMA) {
        Token comma = p->cur_token;
        parser_consume(p);

        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol";
            output_miss_symbol(p, &comma.position, message, strlen(message));

            continue;
        }

        Symbol *symbolValue = symbol_from_token(&p->cur_token);
        parser_consume(p);

        SymbolNode *node = malloc(sizeof(SymbolNode));
        if (node == NULL) {
            UNIMPLEMENTED("for_generic_statement_parse");
        }

        node->value = symbolValue;
        node->next = NULL;

        symbolsCurrent->next = node;
        symbolsCurrent = node;
    }
    lastPos = &symbolsCurrent->value->position;

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

    ForGenericLoopStatement *forGenericLoopStatement = malloc(sizeof(ForGenericLoopStatement));
    if (forGenericLoopStatement == NULL) {
        UNIMPLEMENTED("for_generic_statement_parse");
    }

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
    Token symbolToken = p->cur_token;
    Symbol *symbol;
    if (symbolToken.type != TOKEN_SYMBOL) {
        const char *message = "missing symbol for iterator";
        output_miss_symbol(p, &_for->position, message, strlen(message));
    } else {
        lastPos = &symbolToken.position;
        symbol = symbol_from_token(&symbolToken);
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
        LiteralNumberExpression literalNumberExpression;
        literalNumberExpression.value = 1;

        increment.type = EXPRESSION_LITERAL_NUMBER;
        increment.value = &literalNumberExpression;
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

    ForNumericLoopStatement *forNumericLoopStatement = malloc(sizeof(ForNumericLoopStatement));
    if (forNumericLoopStatement == NULL) {
        UNIMPLEMENTED("for_statement_parse");
    }

    forNumericLoopStatement->symbol = symbol;
    forNumericLoopStatement->initializer = iteratorInitializer;
    forNumericLoopStatement->condition = condition;
    forNumericLoopStatement->increment = increment;
    forNumericLoopStatement->statements = statements;

    statement.type = STATEMENT_FOR_NUMERIC;
    statement.value = forNumericLoopStatement;
    statement.position = position_from_to(&_for->position, lastPos);
    return (Statement){0};
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
            Symbol *symbol = symbol_from_token(&p->cur_token);
            parser_consume(p);

            VariableExpression variableExpression;
            variableExpression.symbol = symbol;

            index.type = EXPRESSION_VARIABLE;
            index.position = symbol->position;
            index.value = &variableExpression;
        }

        while (true) {
            if (p->cur_token.type == TOKEN_DOT) {
                Token dot = p->cur_token;
                parser_consume(p);

                Symbol *symbol = NULL;
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \".\"";
                    output_miss_symbol(p, &dot.position, message, strlen(message));
                } else {
                    symbol = symbol_from_token(&p->cur_token);
                    parser_consume(p);
                }

                Expression valueExpression;

                VariableNameIndexExpression variableNameIndexExpression;
                variableNameIndexExpression.first = &index;
                variableNameIndexExpression.index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX;
                valueExpression.position = symbol->position;
                valueExpression.value = &variableNameIndexExpression;

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

            VariableAssignmentStatement variableAssignmentStatement;
            variableAssignmentStatement.expression = exp;
            variableAssignmentStatement.index = index;

            statement.type = STATEMENT_ASSIGNMENT;
            statement.value = &variableAssignmentStatement;
            statement.position = position_from_to(&token.position, lastPos);
            return statement;
        }

        Expression expression = expression_chain_parse(p, index);

        if (expression.type != EXPRESSION_FUNCTION_CALL) {
            const char *message = "unexpected expression as statement";
            output_unexpected_expression(p, &expression.position, message, strlen(message));
        }

        Statement statement = {0};

        statement.type = STATEMENT_EXPRESSION;
        statement.value = &expression;
        statement.position = expression.position;
        return statement;
    }

    if (token.type == TOKEN_GOTO) {
        parser_consume(p);
        Position *lastPos = &token.position;

        Symbol *symbol = NULL;
        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol after goto point operator";
            output_miss_symbol(p, &token.position, message, strlen(message));
        } else {
            lastPos = &p->cur_token.position;
            symbol = symbol_from_token(&p->cur_token);
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
            return (Statement){0};
        }

        Statement statement = {0};

        LocalStatement local_statement;
        local_statement.child = child;

        statement.type = STATEMENT_LOCAL;
        statement.value = &local_statement;
        statement.position = position_from_to(&token.position, &child.position);
        return statement;
    }

    if (token_is_keyword("function", &token)) {
        parser_consume(p);

        Expression index;
        {
            Symbol *symbol = NULL;
            if (p->cur_token.type != TOKEN_SYMBOL) {
                const char *message = "missing function name";
                output_miss_symbol(p, &token.position, message, strlen(message));
            } else {
                symbol = symbol_from_token(&p->cur_token);
                parser_consume(p);
            }

            VariableExpression variableExpression;
            variableExpression.symbol = symbol;

            index.type = EXPRESSION_VARIABLE;
            index.position = symbol->position;
            index.value = &variableExpression;
        }

        while (true) {
            if (p->cur_token.type == TOKEN_DOT) {
                Token dot = p->cur_token;
                parser_consume(p);

                Symbol *symbol = NULL;
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \".\"";
                    output_miss_symbol(p, &dot.position, message, strlen(message));
                } else {
                    symbol = symbol_from_token(&p->cur_token);
                    parser_consume(p);
                }

                Expression valueExpression;

                VariableNameIndexExpression variableNameIndexExpression;
                variableNameIndexExpression.first = &index;
                variableNameIndexExpression.index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX;
                valueExpression.position = symbol->position;
                valueExpression.value = &variableNameIndexExpression;

                index = valueExpression;
                continue;
            }

            if (p->cur_token.type == TOKEN_COLON) {
                Token colon = p->cur_token;
                parser_consume(p);

                Symbol *symbol = NULL;
                if (p->cur_token.type != TOKEN_SYMBOL) {
                    const char *message = "missing function name index after \":\"";
                    output_miss_symbol(p, &colon.position, message, strlen(message));
                } else {
                    symbol = symbol_from_token(&p->cur_token);
                    parser_consume(p);
                }

                Expression valueExpression;

                VariableNameIndexWithSelfExpression variableNameIndexWithSelfExpression;
                variableNameIndexWithSelfExpression.first = &index;
                variableNameIndexWithSelfExpression.index = symbol;

                valueExpression.type = EXPRESSION_VARIABLE_NAME_INDEX_WITH_SELF;
                valueExpression.position = symbol->position;
                valueExpression.value = &variableNameIndexWithSelfExpression;

                index = valueExpression;
            }

            break;
        }

        Statement statement = {0};

        FunctionDeclaration functionDeclaration;
        functionDeclaration.expression = function_expression_parse(p, &token);
        functionDeclaration.index = index;

        statement.type = STATEMENT_FUNCTION_DECLARATION;
        statement.value = &functionDeclaration;
        statement.position = position_from_to(&token.position, &functionDeclaration.expression.position);
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

        ReturnStatement *returnStatement = malloc(sizeof(ReturnStatement));
        if (returnStatement == NULL) {
            UNIMPLEMENTED("statement_parse");
        }

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

        DoStatement *doStatement = malloc(sizeof(DoStatement));
        if (doStatement == NULL) {
            UNIMPLEMENTED("statement_parse");
        }

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

        RepeatStatement *repeatStatement = malloc(sizeof(RepeatStatement));
        if (repeatStatement == NULL) {
            UNIMPLEMENTED("statement_parse");
        }

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

        WhileStatement *whileStatement = malloc(sizeof(WhileStatement));
        if (whileStatement == NULL) {
            UNIMPLEMENTED("statement_parse");
        }

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

        BreakStatement *breakStatement = malloc(sizeof(BreakStatement));
        if (breakStatement == NULL) {
            UNIMPLEMENTED("statement_parse");
        }


        statement.type = STATEMENT_BREAK;
        statement.position = token.position;
        statement.value = breakStatement;
        return statement;
    }

    if (token_is_keyword("goto", &token)) {
        parser_consume(p);
        Position *lastPos = &token.position;

        Symbol *symbol = NULL;
        if (p->cur_token.type != TOKEN_SYMBOL) {
            const char *message = "missing symbol after \"goto\" symbol";
            output_miss_symbol(p, lastPos, message, strlen(message));
        } else {
            symbol = symbol_from_token(&p->cur_token);
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

    return (Statement){0};
}

StatementNode *statement_parse_body(Parser *p, bool (*isEnd)(Token *token)) {
    StatementNode *statementHead = malloc(sizeof(StatementNode));
    if (statementHead == NULL) {
        UNIMPLEMENTED("statement_parse_body");
    }

    statementHead->value = (Statement){0};
    StatementNode *statementCurrent = statementHead;

    while (true) {
        if (isEnd(&p->cur_token) || p->cur_token.type == TOKEN_END) {
            break;
        }

        Statement statement = statement_parse(p);
        if (statement.type != STATEMENT_NONE) {
            StatementNode *node = malloc(sizeof(StatementNode));
            if (node == NULL) {
                UNIMPLEMENTED("statement_parse_body");
            }

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
