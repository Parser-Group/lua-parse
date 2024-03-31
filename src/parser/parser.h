#ifndef LUA_PARSER_PARSER_H
#define LUA_PARSER_PARSER_H
#include "lexer/lexer.h"

#include "parser_structs.h"
#include "statement.h"
#include "statement_structs.h"

Parser parser_new(Lexer *l, void (*onOutput) (Position *position, OutputCode code, const char *message, size_t message_len));
void parser_consume(Parser *p);
void parser_rollback(Parser *p);
Statement parser_next(Parser *p);

#endif //LUA_PARSER_PARSER_H
