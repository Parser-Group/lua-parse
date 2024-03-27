#ifndef LUA_PARSER_OUTPUT_H
#define LUA_PARSER_OUTPUT_H

#include "parser/parser.h"

void output_miss_open_paren(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_close_paren(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_open_bracket(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_close_bracket(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_open_brace(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_close_brace(Parser *p, Position *pos, const char *message, size_t message_len);

void output_miss_symbol(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_expression(Parser *p, Position *pos, const char *message, size_t message_len);
void output_miss_end(Parser *p, Position *pos, const char *message, size_t message_len);

void output_unexpected_token(Parser *p, Position *pos, const char *message, size_t message_len);
void output_unexpected_keyword(Parser *p, Position *pos, const char *message, size_t message_len);

#endif //LUA_PARSER_OUTPUT_H
