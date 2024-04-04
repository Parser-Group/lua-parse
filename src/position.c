#include "position.h"

Position position_new(size_t start_line, size_t start_column, size_t end_line, size_t end_column) {
    Position position = {0};
    position.start_line = start_line;
    position.start_column = start_column;
    position.end_line = end_line;
    position.end_column = end_column;
    return position;
}

Position position_from_to(Position *start, Position *end) {
    return position_new(start->start_line, start->start_column, end->end_line, end->end_column);
}

Position position_after(Position *pos) {
    return position_new(pos->end_line, pos->end_column + 1, pos->end_column, pos->end_column + 1);
}

/// returns NULL when unable to allocate string. Return value (char *) needs to be freed.
char* position_to_string(Position *pos) {
    int size = snprintf(NULL, 0, "<%zu:%zu-%zu:%zu>", pos->start_line, pos->start_column, pos->end_line, pos->end_column) + 1;
    char *buffer = malloc(size);
    if (buffer == NULL) {
        printf("%s:%d: Memory Allocation failed\n", __FILE__, __LINE__);
        exit(1);
    }
    
    snprintf(buffer, size, "<%zu:%zu-%zu:%zu>", pos->start_line, pos->start_column, pos->end_line, pos->end_column);
    return buffer;
}
