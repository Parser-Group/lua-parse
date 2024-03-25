#include "position.h"

Position position_from_to(Position *start, Position *end) {
    Position pos;
    pos.start_line = start->start_line;
    pos.start_column = start->start_column;
    pos.end_line = end->end_line;
    pos.end_column = end->end_column;
    return pos;
}