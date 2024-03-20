#ifndef LUATRANSPILER_POSITION_H
#define LUATRANSPILER_POSITION_H

typedef struct {
    size_t start_line;
    size_t end_line;
    size_t start_column;
    size_t end_column;
} Position;

#endif //LUATRANSPILER_POSITION_H
