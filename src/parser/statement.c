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
    if (keyword_is("function", p->cur_token)) {
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
        
        statement.type = STATEMENT_FUNCTION;
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
            statement.type = STATEMENT_DECLARATION_ASSIGNMENT;
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
        statement.type = STATEMENT_DECLARATION_ASSIGNMENT;
        statement.value = &variableDeclaration;
        return statement;
    }
    
    return statement_empty();
}

Statement statement_parse(Parser *p) {
    Token token = p->cur_token;
    
    if (keyword_is("local", token)) {
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

    output_unexpected_token(p, &token.position, token.text, token.text_len);
    
    return statement_empty();
}