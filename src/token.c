#include <memory.h>
#include "token.h"

const char *token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_END:
            return "end of content";
        case TOKEN_INVALID:
            return "invalid token";
        case TOKEN_SYMBOL:
            return "symbol";
        case TOKEN_KEYWORD:
            return "keyword";
        case TOKEN_COMMENT:
            return "comment";

        case TOKEN_STRING:
            return "string";
        case TOKEN_NUMBER:
            return "number";
            
        case TOKEN_OPEN_PAREN:
            return "open paren";
        case TOKEN_CLOSE_PAREN:
            return "close paren";
        case TOKEN_OPEN_BRACE:
            return "open brace";
        case TOKEN_CLOSE_BRACE:
            return "close brace";
        case TOKEN_OPEN_BRACKET:
            return "open bracket";
        case TOKEN_CLOSE_BRACKET:
            return "close bracket";

        case TOKEN_DOT:
            return "dot";
        case TOKEN_COLON:
            return "colon";
        case TOKEN_SEMICOLON:
            return "semicolon";
        case TOKEN_COMMA:
            return "comma";
            
        case TOKEN_BIT_NOT:
            return "bit not";
        case TOKEN_BIT_OR:
            return "bit or";
        case TOKEN_BIT_AND:
            return "bit and";
        case TOKEN_BIT_SHIFT_LEFT:
            return "bit shift left";
        case TOKEN_BIT_SHIFT_RIGHT:
            return "bit shift right";
            
        case TOKEN_EQUALS:
            return "equals";
        case TOKEN_NOT_EQUALS:
            return "not equals";
        case TOKEN_GREATER_THAN:
            return "greater than";
        case TOKEN_GREATER_THAN_OR_EQUAL:
            return "greater than or equal";
        case TOKEN_LESS_THAN:
            return "less than";
        case TOKEN_LESS_THAN_OR_EQUAL:
            return "less than or equal";

        case TOKEN_GOTO:
            return "goto";
        case TOKEN_STRING_CONCAT:
            return "string concat";
        case TOKEN_VAR_ARG:
            return "var arg";
            
        case TOKEN_EQUAL:
            return "equal";
        case TOKEN_PLUS:
            return "plus";
        case TOKEN_MINUS:
            return "minus";
        case TOKEN_MULTIPLY:
            return "multiply";
        case TOKEN_DIVIDE:
            return "divide";
        case TOKEN_POWER:
            return "power";
        case TOKEN_MODULO:
            return "modulo";
        case TOKEN_LENGTH:
            return "hash";
            
        default:
            UNREACHABLE("token_type_name");
    }
}

char *token_to_string(Token *token) {
    int size = snprintf(NULL, 0, "%.*s \'%s\'", (int)token->text_len, token->text, token_type_name(token->type)) + 1;
    char *buffer = malloc(size);
    
    if (buffer == NULL) {
        return NULL;
    }

    sprintf(buffer, "%.*s \'%s\'", (int)token->text_len, token->text, token_type_name(token->type));
    return buffer;
}
