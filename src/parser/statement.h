#ifndef LUA_PARSER_STATEMENT_H
#define LUA_PARSER_STATEMENT_H

#include "parser.h"
#include "statement_structs.h"

Statement* statement_new();
Statement* statement_parse(Parser *p);
StatementNode* statement_parse_body(Parser *p, bool (*isEnd)(Token *token));

#endif //LUA_PARSER_STATEMENT_H
