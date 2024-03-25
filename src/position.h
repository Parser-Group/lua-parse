#ifndef LUA_PARSER_POSITION_H
#define LUA_PARSER_POSITION_H

#include "common.h"

typedef struct {
    size_t start_line;
    size_t end_line;
    size_t start_column;
    size_t end_column;
} Position;

Position position_from_to(Position *start, Position *end);

#endif //LUA_PARSER_POSITION_H
