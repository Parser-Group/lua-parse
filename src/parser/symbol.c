#include "symbol.h"
#include "common.h"

Symbol* symbol_new(Position *position, const char *value, size_t value_len) {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol == nullptr) {
        UNIMPLEMENTED("symbol_new");
    }
    
    symbol->position = position;
    symbol->value = value;
    symbol->value_len = value_len;
    return symbol;
}

Symbol* symbol_from_token(Token *token) {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol == nullptr) {
        UNIMPLEMENTED("symbol_from_token");
    }
    
    symbol->position = token->position;
    symbol->value = token->text;
    symbol->value_len = token->text_len;
    
    return symbol;
}