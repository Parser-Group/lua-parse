#include "symbol.h"
#include "common.h"

Symbol* symbol_from_token(Token *token) {
    SAFE_MALLOC(Symbol, symbol)
    
    symbol->position = token->position;
    symbol->value = token->text;
    symbol->value_len = token->text_len;
    
    return symbol;
}