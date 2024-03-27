#include "position.h"

Position position_from_to(Position *start, Position *end) {
    Position pos;
    pos.start_line = start->start_line;
    pos.start_column = start->start_column;
    pos.end_line = end->end_line;
    pos.end_column = end->end_column;
    return pos;
}

Position position_after(Position *pos) {
    Position posAfter;
    posAfter.start_line = pos->end_line;
    posAfter.start_column = pos->end_column + 1;
    posAfter.end_line = pos->end_column;
    posAfter.end_column = pos->end_column + 1;
    return posAfter;
}

/// returns NULL when unable to allocate string. Return value (char *) needs to be freed.
char* position_to_string(Position *pos) {
    int size = snprintf(NULL, 0, "<%zu:%zu-%zu:%zu>", pos->start_line, pos->start_column, pos->end_line, pos->end_column) + 1;
    char *buffer = malloc(size);
    
    if (buffer == NULL) {
        return NULL;
    }
    
    snprintf(buffer, size, "<%zu:%zu-%zu:%zu>", pos->start_line, pos->start_column, pos->end_line, pos->end_column);
    return buffer;
}
