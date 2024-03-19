#ifndef LUATRANSPILER_CHAR_POSITION_H
#define LUATRANSPILER_CHAR_POSITION_H
#include "types.h"
#include "Position.h"

typedef struct {
    uSize line;
    uSize column;
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

#endif //LUATRANSPILER_CHAR_POSITION_H
