#ifndef LUA_PARSER_SYMBOL_H
#define LUA_PARSER_SYMBOL_H

#include "position.h"
#include "token.h"

typedef struct {
    Position *position;
    const char *value;
    size_t value_len;
} Symbol;

typedef struct SymbolNode {
    struct SymbolNode *next;
    Symbol *value;
} SymbolNode;

Symbol* symbol_empty();
Symbol* symbol_new(Position *position, const char *value, size_t value_len);
Symbol* symbol_from_token(Token *token);

#endif //LUA_PARSER_SYMBOL_H
