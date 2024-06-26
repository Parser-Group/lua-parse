#ifndef LUA_PARSER_OUTPUT_H
#define LUA_PARSER_OUTPUT_H

#define OUTPUT_METHOD(funcName) void funcName(Parser *p, Position *pos, const char *message, size_t message_len);

#include "parser/parser.h"

OUTPUT_METHOD(output_miss_open_paren)
OUTPUT_METHOD(output_miss_close_paren)
OUTPUT_METHOD(output_miss_open_bracket)
OUTPUT_METHOD(output_miss_close_bracket)
OUTPUT_METHOD(output_miss_open_brace)
OUTPUT_METHOD(output_miss_close_brace)

OUTPUT_METHOD(output_miss_comma)
OUTPUT_METHOD(output_miss_equal)

OUTPUT_METHOD(output_miss_symbol)
OUTPUT_METHOD(output_miss_expression)
OUTPUT_METHOD(output_miss_keyword_end)
OUTPUT_METHOD(output_miss_keyword_then)
OUTPUT_METHOD(output_miss_keyword_do)
OUTPUT_METHOD(output_miss_keyword_in)
OUTPUT_METHOD(output_miss_goto_point)

/// message has to include one '%s' for the token data to go 
void output_unexpected_token(Parser *p, Token *token, const char *message);
OUTPUT_METHOD(output_unexpected_keyword)
OUTPUT_METHOD(output_unexpected_expression)

#endif //LUA_PARSER_OUTPUT_H
