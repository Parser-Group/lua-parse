#ifndef LUA_PARSER_EXPRESSIONS_H
#define LUA_PARSER_EXPRESSIONS_H

#include "parser.h"
#include "expression_structs.h"

Expression expression_empty();
Expression function_expression_parse(Parser *p, Token func);
Expression expression_parse(Parser *p);

#endif //LUA_PARSER_EXPRESSIONS_H
