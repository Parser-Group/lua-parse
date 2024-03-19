#ifndef LUATRANSPILER_POSITION_H
#define LUATRANSPILER_POSITION_H
#include "types.h"

typedef struct {
    uSize start_line;
    uSize end_line;
    uSize start_column;
    uSize end_column;
} Position;

#endif //LUATRANSPILER_POSITION_H
