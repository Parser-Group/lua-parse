#include <string.h>
#include "statement.h"
#include "utils.h"
#include "output.h"

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
        
    }
}

Statement statement_parse(Parser *p) {
    
    if (keyword_is("local", p->cur_token)) {
        Token local = p->cur_token;
        parser_consume(p);
        
        Statement statement;
        
        LocalStatement local_statement;
        local_statement.parent = &statement;
        local_statement.child = local_statement_parse(p);
        
        statement.type = STATEMENT_LOCAL;
        statement.value = &local_statement;
        statement.position = position_from_to(&local.position, &local_statement.child.position);
        return statement;
    }
    
}