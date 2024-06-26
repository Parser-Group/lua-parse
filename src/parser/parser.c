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
    Token next = lexer_next(p->lexer);
    while (next.type == TOKEN_COMMENT || next.type == TOKEN_SEMICOLON) {
        next = lexer_next(p->lexer);
    }
    p->cur_token = next;
}

void parser_rollback(Parser *p) {
    p->lexer->cursor = p->lexer->begin_of_line + p->cur_token.position.start_column - 1;
}

Statement parser_next(Parser *p) {
    return statement_parse(p);
}
