#ifndef LUA_PARSER_OUTPUT_H
#define LUA_PARSER_OUTPUT_H

#include "parser/parser.h"

void output_miss_paren(Parser *p, Position *pos);
void output_miss_bracket(Parser *p, Position *pos);
void output_miss_brace(Parser *p, Position *pos);

#endif //LUA_PARSER_OUTPUT_H
