#ifndef LUAPARSER_POSITION_H
#define LUAPARSER_POSITION_H

typedef struct {
    size_t start_line;
    size_t end_line;
    size_t start_column;
    size_t end_column;
} Position;

#endif //LUAPARSER_POSITION_H
