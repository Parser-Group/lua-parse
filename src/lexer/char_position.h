#ifndef LUA_LEXER_CHAR_POSITION_H
#define LUA_LEXER_CHAR_POSITION_H

typedef struct {
    size_t line;
    size_t column;
} CharPosition;

Position char_position_from_to(CharPosition start, CharPosition end) {
    return position_new(start.line, start.column, end.line, end.column);
}

Position char_position_to_position(CharPosition pos) {
    return position_new(pos.line, pos.column, pos.line, pos.column);
}

#endif //LUA_LEXER_CHAR_POSITION_H
