#include "parser.h"
#include "statement.h"

Parser parser_new(Lexer *l) {
    Parser parser;
    parser.lexer = l;
    parser.cur_token = lexer_next(l);
    return parser;
}

<<<<<<< HEAD
Statement parser_next(Parser *p) {
    Statement statement = {0};

    Token token = lexer_next(p->lexer);
    Position start_pos = token.position;

    if (token_is_keyword("local", &token)) {
        token = lexer_next(p->lexer);

        if (token.type == TOKEN_SYMBOL) {
            //TODO: variable declaration

            token = lexer_next(p->lexer);

        }

        if (token_is_keyword("function", &token)) {
            //TODO: function declaration
        }
    }
}
=======
void parser_consume(Parser *p) {
    p->cur_token = lexer_next(p->lexer);
}

Statement parser_next(Parser *p) {
    return statement_parse(p);
}
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7
