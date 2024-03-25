#ifndef LUAPARSER_PARSER_H
#define LUAPARSER_PARSER_H

#include "parser_structs.h"
#include "statement.h"
#include "statement_structs.h"

Parser parser_new(Lexer *l);
void parser_consume(Parser *p);

Statement parser_next(Parser *p);

#endif //LUAPARSER_PARSER_H
