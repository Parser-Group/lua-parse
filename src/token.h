#ifndef LUA_PARSER_TOKEN_H
#define LUA_PARSER_TOKEN_H

#include "common.h"
#include "position.h"

typedef enum {
    TOKEN_END = 0,

    TOKEN_BIT_NOT = 10,
    TOKEN_BIT_OR = 11,
    TOKEN_BIT_AND = 12,
    TOKEN_BIT_SHIFT_LEFT = 13,
    TOKEN_BIT_SHIFT_RIGHT = 14,

    TOKEN_EQUALS = 20,
    TOKEN_NOT_EQUALS = 21,
    TOKEN_GREATER_THAN = 22,
    TOKEN_GREATER_THAN_OR_EQUAL = 23,
    TOKEN_LESS_THAN = 24,
    TOKEN_LESS_THAN_OR_EQUAL = 25,

    TOKEN_STRING_CONCAT = 30,

    TOKEN_PLUS = 41,
    TOKEN_MINUS = 42,
    TOKEN_MULTIPLY = 43,
    TOKEN_DIVIDE = 44,
    TOKEN_POWER = 45,
    TOKEN_MODULO = 46,
    
    TOKEN_INVALID,
    TOKEN_SYMBOL,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,

    TOKEN_NUMBER,
    TOKEN_STRING,
    
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
    TOKEN_EQUAL,
    
    TOKEN_GOTO,
    TOKEN_VAR_ARG,
    TOKEN_HASH,
} TokenType;

const char *token_type_name(TokenType type);

typedef struct Token {
    Position position;
    TokenType type;
    const char *text;
    size_t text_len;
} Token;

char *token_to_string(Token *token);

#endif //LUA_PARSER_TOKEN_H
