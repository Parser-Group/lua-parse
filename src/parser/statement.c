#include <string.h>
#include "statement.h"
#include "utils.h"
#include "output.h"

Statement *statement_new() {
    Statement *statement = malloc(sizeof(Statement));
    statement->type = STATEMENT_NONE;
    return statement;
}

Statement *statement_empty() {
    Statement *statement = malloc(sizeof(Statement));
    statement->type = STATEMENT_NONE;
    return statement;
}

Statement *local_statement_parse(Parser *p) {
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

        Statement *statement = statement_new();

        FunctionDeclaration functionDeclaration;
        functionDeclaration.parent = statement;
        functionDeclaration.expression = function_expression_parse(p, func);
        functionDeclaration.symbol = funcName.text;
        functionDeclaration.symbol_len = funcName.text_len;

        statement->type = STATEMENT_FUNCTION_DECLARATION;
        statement->value = &functionDeclaration;
        statement->position = position_from_to(&func.position, &functionDeclaration.expression.position);
        return statement;
    }

    if (p->cur_token.type == TOKEN_SYMBOL) {
        Token symbol = p->cur_token;
        parser_consume(p);

        Token equal = p->cur_token;
        if (equal.type != TOKEN_EQUAL) {
            Statement *statement = statement_new();

            VariableDeclaration variableDeclaration;
            variableDeclaration.parent = statement;
            variableDeclaration.initializer = nullptr;
            variableDeclaration.symbol = symbol.text;
            variableDeclaration.symbol_len = symbol.text_len;

            statement->position = symbol.position;
            statement->type = STATEMENT_VARIABLE_DECLARATION;
            statement->value = &variableDeclaration;
            return statement;
        } else {
            parser_consume(p);
        }

        Expression initializer = expression_parse(p);
        if (initializer.type == EXPRESSION_NONE) {
            const char *message = "missing expression as initializer for variable declaration";
            output_miss_expression(p, &equal.position, message, strlen(message));
        }

        Statement *statement = statement_new();

        VariableDeclaration variableDeclaration;
        variableDeclaration.parent = statement;
        variableDeclaration.initializer = &initializer;
        variableDeclaration.symbol = symbol.text;
        variableDeclaration.symbol_len = symbol.text_len;

        statement->position = position_from_to(&symbol.position, &initializer.position);
        statement->type = STATEMENT_VARIABLE_DECLARATION;
        statement->value = &variableDeclaration;
        return statement;
    }

    return statement_empty();
}

bool is_end_of_if_body(Token *token) {
    return token_is_keyword("end", token) || token_is_keyword("elseif", token) || token_is_keyword("else", token);
}

Statement *statement_parse(Parser *p) {
    Token token = p->cur_token;

    Expression exp = expression_parse(p);
    if (exp.type != EXPRESSION_NONE) {
        if (exp.type != EXPRESSION_FUNCTION_CALL) {
            const char *message = "unexpected expression as statement";
            output_unexpected_expression(p, &exp.position, message, strlen(message));
        }

        Statement *statement = statement_new();

        ExpressionStatement expressionStatement;
        expressionStatement.parent = statement;
        expressionStatement.expression = exp;

        statement->type = STATEMENT_EXPRESSION;
        statement->value = &expressionStatement;
        statement->position = exp.position;
        return statement;
    }

    if (token.type == TOKEN_SYMBOL) {
        parser_consume(p);
        Token nextToken = p->cur_token;

        if (nextToken.type == TOKEN_EQUAL) {
            parser_consume(p);

            Position lastPos;

            exp = expression_parse(p);
            if (exp.type == EXPRESSION_NONE) {
                const char *message = "";
                output_miss_expression(p, &nextToken.position, message, strlen(message));

                lastPos = nextToken.position;
            } else {
                lastPos = exp.position;
            }

            Statement *statement = statement_new();

            VariableAssignmentStatement variableAssignmentStatement;
            variableAssignmentStatement.parent = statement;
            variableAssignmentStatement.expression = exp;
            variableAssignmentStatement.symbol = token.text;
            variableAssignmentStatement.symbol_len = token.text_len;

            statement->type = STATEMENT_ASSIGNMENT;
            statement->value = &variableAssignmentStatement;
            statement->position = position_from_to(&token.position, &lastPos);
            return statement;
        }
    }

    if (token_is_keyword("local", &token)) {
        parser_consume(p);

        Statement *child = local_statement_parse(p);
        if (child->type == STATEMENT_NONE) {
            const char *message = "unexpected \"local\" keyword";
            output_unexpected_token(p, &token.position, message, strlen(message));
            return statement_empty();
        }

        Statement *statement = statement_new();

        LocalStatement local_statement;
        local_statement.parent = statement;
        local_statement.child = child;

        statement->type = STATEMENT_LOCAL;
        statement->value = &local_statement;
        statement->position = position_from_to(&token.position, &child->position);
        return statement;
    }

    if (token_is_keyword("if", &token)) {
        Token _if = p->cur_token;
        parser_consume(p);

        Expression condition = expression_parse(p);
        if (condition.type == EXPRESSION_NONE) {
            const char *message = "missing if condition";
            output_miss_expression(p, &_if.position, message, strlen(message));
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
        elseIfBodyStatementsHead->next = nullptr;
        elseIfBodyStatementsHead->value = nullptr;
        ElseStatementNode *elseBodyStatementsHead = malloc(sizeof(ElseStatementNode));
        elseBodyStatementsHead->next = nullptr;
        elseBodyStatementsHead->value = nullptr;
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
                    elseIfStatement->statements = statement_parse_body(p, is_end_of_if_body);
                    elseIfStatement->condition = elseIfCondition;
                    
                    ElseIfStatementNode *node = malloc(sizeof(ElseIfStatementNode));
                    node->value = elseIfStatement;
                    node->next = nullptr;

                    elseIfBodyStatements->next = node;
                    elseIfBodyStatements = node;
                    continue;
                }

                if (token_is_keyword("else", &elseToken)) {
                    parser_consume(p);

                    ElseStatement *elseStatement = malloc(sizeof(ElseStatement));
                    elseStatement->statements = statement_parse_body(p, is_end_of_if_body);
                    
                    ElseStatementNode *node = malloc(sizeof(ElseStatementNode));
                    node->value = elseStatement;
                    node->next = nullptr;

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
            output_miss_keyword_end(p, &_if.position, message, strlen(message));
        } else {
            parser_consume(p);
        }

        Statement *statement = statement_new();

        IfStatement _ifStatement;
        _ifStatement.parent = statement;
        _ifStatement.condition = condition;
        _ifStatement.if_body = ifBodyStatements;
        //TODO: finish else and else if statements
        //        _ifStatement.else_if_body_nodes = elseIfBodyStatementsHead->next;
        //        free(elseIfBodyStatementsHead);
        //        _ifStatement.else_body_nodes = elseBodyStatementsHead->next;
        //        free(elseBodyStatementsHead);

        statement->type = STATEMENT_IF;
        statement->value = &_ifStatement;
        statement->position = position_from_to(&_if.position, &end.position);
        return statement;
    }

    if (token.type == TOKEN_END) {
        Statement *statement = statement_new();
        statement->type = STATEMENT_END;
        return statement;
    }

    const char *message = token_to_string(&token);
    output_unexpected_token(p, &token.position, message, strlen(message));
    parser_consume(p);

    return statement_empty();
}

StatementNode *statement_parse_body(Parser *p, bool (*isEnd)(Token *token)) {
    StatementNode *statementHead = malloc(sizeof(StatementNode));
    statementHead->value = nullptr;
    StatementNode *statementCurrent = statementHead;

    while (true) {
        if (isEnd(&p->cur_token) || p->cur_token.type == TOKEN_END) {
            break;
        }

        Statement *statement = statement_parse(p);
        if (statement->type != STATEMENT_NONE) {
            StatementNode *node = malloc(sizeof(StatementNode));
            node->value = statement;
            node->next = nullptr;

            statementCurrent->next = node;
            statementCurrent = node;
        }
        else {
            free(statement);    
        }
    }

    statementCurrent = statementHead->next;
    free(statementHead);
    return statementCurrent;
}
