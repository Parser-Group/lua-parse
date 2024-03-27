#include "parser.h"
#include "statement.h"

Parser parser_new(Lexer *l, void (*onOutput) (Position *position, OutputCode code, const char *message, size_t message_len)) {
    Parser parser;
    parser.lexer = l;
    parser.onOutput = onOutput;
    parser_consume(&parser);
    return parser;
}

void parser_consume(Parser *p) {
    Token next = lexer_next(p->lexer);;
    while (next.type == TOKEN_COMMENT) {
        next = lexer_next(p->lexer);
    }
    p->cur_token = next;
}

Statement parser_next(Parser *p) {
    return statement_parse(p);
}
