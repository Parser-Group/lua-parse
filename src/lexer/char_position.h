#ifndef LUAPARSER_CHAR_POSITION_H
#define LUAPARSER_CHAR_POSITION_H
#include "position.h"

typedef struct {
    size_t line;
    size_t column;
} CharPosition;

Position char_position_from_to(CharPosition start, CharPosition end) {
    Position position;
    position.start_line = start.line;
    position.start_column = start.column;
    position.end_line = end.line;
    position.end_column = end.column;
    return position;
}

Position char_position_to_position(CharPosition pos) {
    Position position;
    position.start_line = pos.line;
    position.end_line = pos.line;
    position.start_column = pos.column;
    position.end_column = pos.column;
    return position;
}

#endif //LUAPARSER_CHAR_POSITION_H
