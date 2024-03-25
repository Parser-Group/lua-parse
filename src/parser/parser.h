<<<<<<< HEAD
#ifndef LUA_PARSER_PARSER_H
#define LUA_PARSER_PARSER_H
#include "lexer/lexer.h"
=======
#ifndef LUAPARSER_PARSER_H
#define LUAPARSER_PARSER_H
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7

#include "parser_structs.h"
#include "statement.h"
#include "statement_structs.h"

Parser parser_new(Lexer *l);
void parser_consume(Parser *p);

Statement parser_next(Parser *p);

#endif //LUA_PARSER_PARSER_H
