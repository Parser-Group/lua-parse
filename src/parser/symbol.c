#include "symbol.h"
#include "common.h"

Symbol* symbol_from_token(Token *token) {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol == NULL) {
        UNIMPLEMENTED("symbol_from_token");
    }
    
    symbol->position = token->position;
    symbol->value = token->text;
    symbol->value_len = token->text_len;
    
    return symbol;
}