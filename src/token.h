#ifndef LUAPARSER_TOKEN_H
#define LUAPARSER_TOKEN_H

#include "utils.h"
#include "position.h"

typedef enum {
    TOKEN_END = 0,
    TOKEN_INVALID,
    TOKEN_SYMBOL,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,

    TOKEN_STRING,
    TOKEN_NUMBER,
    
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,

    TOKEN_DOT,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    
    TOKEN_BIT_NOT,
    TOKEN_BIT_OR,
    TOKEN_BIT_AND,
    TOKEN_BIT_SHIFT_LEFT,
    TOKEN_BIT_SHIFT_RIGHT,
    
    TOKEN_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_THAN_OR_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_LESS_THAN_OR_EQUAL,
    
    TOKEN_GOTO,
    TOKEN_STRING_CONCAT,
    TOKEN_VAR_ARG,
    
    TOKEN_EQUAL,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_TIMES,
    TOKEN_DIVIDE,
    TOKEN_POWER,
    TOKEN_MODULO,
    TOKEN_LENGTH,

} TokenType;

const char *token_type_name(TokenType type);

typedef struct {
    Position position;
    TokenType type;
    const char *text;
    size_t text_len;
} Token;

#endif //LUAPARSER_TOKEN_H
