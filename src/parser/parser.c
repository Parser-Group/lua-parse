#include "parser.h"
#include "statement.h"

Parser parser_new(Lexer *l) {
    Parser parser;
    parser.lexer = l;
    return parser;
}

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