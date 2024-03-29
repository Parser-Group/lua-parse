#include "symbol.h"
#include "common.h"

Symbol* symbol_empty() {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol == nullptr) {
        UNIMPLEMENTED("symbol_empty");
    }
    
    symbol->position = (Position){0};
    symbol->value = nullptr;
    symbol->value_len = 0;
    return symbol;
}

Symbol* symbol_new(Position position, const char *value, size_t value_len) {
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

void token_to_symbol(Symbol *symbol, Token *token) {
    symbol->position = token->position;
    symbol->value = token->text;
    symbol->value_len = token->text_len;
}
