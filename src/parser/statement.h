#ifndef LUA_PARSER_STATEMENT_H
#define LUA_PARSER_STATEMENT_H

<<<<<<< HEAD
#include "parser.h"

typedef enum {
    STATEMENT_END = 0,

    // local
    STATEMENT_LOCAL_VARIABLE_DECLARATION,
    STATEMENT_LOCAL_FUNCTION_DECLARATION,
    
    } StatementType;

typedef struct {
    Position position;
    StatementType type;
    void *value;
} Statement;

Statement parser_statement(Parser *p);
=======
#include "position.h"
#include "parser.h"

Statement statement_parse(Parser *p);
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7

#endif //LUA_PARSER_STATEMENT_H
