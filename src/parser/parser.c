#include "parser.h"

Parser parser_new(Lexer *l) {
    Parser parser;
    parser.lexer = l;
    parser.cur_token = lexer_next(l);
    return parser;
}

void parser_consume(Parser *p) {
    p->cur_token = lexer_next(p->lexer);
}

Statement parser_next(Parser *p) {
    return statement_parse(p);
}
