#include <string.h>
#include "statement.h"
#include "utils.h"
#include "output.h"

Statement statement_empty() {
    Statement statement;
    statement.type = STATEMENT_NONE;
    return statement;
}

Statement local_statement_parse(Parser *p) {
    if (token_is_keyword("function", &p->cur_token)) {
        Token func = p->cur_token;
        parser_consume(p);

        Token funcName = p->cur_token;
        if (funcName.type != TOKEN_SYMBOL) {
            const char* message = "missing function name";
            output_miss_symbol(p, &func.position, message, strlen(message));
        }
        else {
            parser_consume(p);
        }
        
        Statement statement;
        
        FunctionDeclaration functionDeclaration;
        functionDeclaration.parent = &statement;
        functionDeclaration.expression = function_expression_parse(p, func);
        functionDeclaration.symbol = funcName.text;
        functionDeclaration.symbol_len = funcName.text_len;
        
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
            Statement statement;
            
            VariableDeclaration variableDeclaration;
            variableDeclaration.parent = &statement;
            variableDeclaration.initializer = NULL;
            variableDeclaration.symbol = symbol.text;
            variableDeclaration.symbol_len = symbol.text_len;

            statement.position = symbol.position;
            statement.type = STATEMENT_VARIABLE_DECLARATION;
            statement.value = &variableDeclaration;
            return statement;
        }
        else {
            parser_consume(p);
        }
        
        Expression initializer = expression_parse(p);
        if (initializer.type == EXPRESSION_NONE) {
            const char *message = "missing expression as initializer for variable declaration";
            output_miss_expression(p, &equal.position, message, strlen(message));
        }
        
        Statement statement;

        VariableDeclaration variableDeclaration;
        variableDeclaration.parent = &statement;
        variableDeclaration.initializer = &initializer;
        variableDeclaration.symbol = symbol.text;
        variableDeclaration.symbol_len = symbol.text_len;

        statement.position = position_from_to(&symbol.position, &initializer.position);
        statement.type = STATEMENT_VARIABLE_DECLARATION;
        statement.value = &variableDeclaration;
        return statement;
    }
    
    return statement_empty();
}

Statement statement_parse(Parser *p) {
    Token token = p->cur_token;

    Expression exp = expression_parse(p);
    if (exp.type != EXPRESSION_NONE) {
        if (exp.type != EXPRESSION_FUNCTION_CALL) {
            const char *message = "unexpected expression as statement";
            output_unexpected_expression(p, &exp.position, message, strlen(message));
        }

        Statement statement;

        ExpressionStatement expressionStatement;
        expressionStatement.parent = &statement;
        expressionStatement.expression = exp;

        statement.type = STATEMENT_EXPRESSION;
        statement.value = &expressionStatement;
        statement.position = exp.position;
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
            }
            else {
                lastPos = exp.position;
            }
            
            Statement statement;
            
            VariableAssignmentStatement variableAssignmentStatement;
            variableAssignmentStatement.parent = &statement;
            variableAssignmentStatement.expression = exp;
            variableAssignmentStatement.symbol = token.text;
            variableAssignmentStatement.symbol_len = token.text_len;
            
            statement.type = STATEMENT_ASSIGNMENT;
            statement.value = &variableAssignmentStatement;
            statement.position = position_from_to(&token.position, &lastPos);
            return statement;
        }
    }
    
    if (token_is_keyword("local", &token)) {
        parser_consume(p);
        
        Statement child = local_statement_parse(p);
        if (child.type == STATEMENT_NONE) {
            const char *message = "";
            output_unexpected_token(p, &token.position, message, strlen(message));
            return statement_empty();
        }
        
        Statement statement;
        
        LocalStatement local_statement;
        local_statement.parent = &statement;
        local_statement.child = child;
        
        statement.type = STATEMENT_LOCAL;
        statement.value = &local_statement;
        statement.position = position_from_to(&token.position, &local_statement.child.position);
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
        }
        else {
            parser_consume(p);
        }
        
        StatementNode statementHead = {0};
        StatementNode statementCurrent = statementHead;
        while (true) {
            if (token_is_keyword("end", &p->cur_token) || p->cur_token.type == TOKEN_END) {
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
        
        Token end = p->cur_token;
        if (!token_is_keyword("end", &p->cur_token)) {
            const char *message = "missing \"end\" of if statement";
            output_miss_keyword_end(p, &_if.position, message, strlen(message));
        }
        else {
            parser_consume(p);
        }
        
        Statement statement;
        
        IfStatement _ifStatement;
        _ifStatement.parent = &statement;
        _ifStatement.condition = condition;
        _ifStatement.statements = statementHead.next;
        
        statement.type = STATEMENT_IF;
        statement.value = &_ifStatement;
        statement.position = position_from_to(&_if.position, &end.position);
        return statement;
    }
    
    if (token.type == TOKEN_END) {
        Statement statement = {0};
        statement.type = STATEMENT_END;
        return statement;
    }
    
    const char *message = token_to_string(&token);
    output_unexpected_token(p, &token.position, message, strlen(message));
    parser_consume(p);
    
    return statement_empty();
}