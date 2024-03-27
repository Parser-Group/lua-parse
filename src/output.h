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

OUTPUT_METHOD(output_miss_symbol)
OUTPUT_METHOD(output_miss_expression)
OUTPUT_METHOD(output_miss_keyword_end)
OUTPUT_METHOD(output_miss_keyword_then)

OUTPUT_METHOD(output_unexpected_token)
OUTPUT_METHOD(output_unexpected_keyword)
OUTPUT_METHOD(output_unexpected_expression)

#endif //LUA_PARSER_OUTPUT_H
