#include "statement.h"
<<<<<<< HEAD
#include "variable_declaration.h"

Statement parser_statement(Parser *p) {
    Statement statement = {0};

    Token token = lexer_next(p->lexer);
    Position start_pos = token.position;

    if (token_is_keyword("local", &token)) {
        token = lexer_next(p->lexer);

        if (token.type == TOKEN_SYMBOL) {
            VariableDeclaration var = parse_variable_declaration(p, token);

            statement.type = STATEMENT_LOCAL_VARIABLE_DECLARATION;
            statement.position = position_from_to(start_pos, )
            return statement;
        }

        if (token_is_keyword("function", &token)) {
            //TODO: function declaration
        }
    }


=======
#include "utils.h"

Statement statement_parse(Parser *p) {
    
    if (keyword_is("local", p->cur_token)) {
        
    }
    
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7
}