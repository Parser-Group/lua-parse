<<<<<<< HEAD
#ifndef LUA_PARSER_POSITION_H
#define LUA_PARSER_POSITION_H

#include "common.h"
=======
#ifndef LUAPARSER_POSITION_H
#define LUAPARSER_POSITION_H
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7

typedef struct {
    size_t start_line;
    size_t end_line;
    size_t start_column;
    size_t end_column;
} Position;

<<<<<<< HEAD
Position position_from_to(Position *start, Position *end);

#endif //LUA_PARSER_POSITION_H
=======
#endif //LUAPARSER_POSITION_H
>>>>>>> 1cdf131b447ff24272bf29e9532aab39b9b697d7
