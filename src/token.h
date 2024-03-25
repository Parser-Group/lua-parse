#ifndef LUA_PARSER_TOKEN_H
#define LUA_PARSER_TOKEN_H

#include "common.h"
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
    TOKEN_EQUAL,
    
    TOKEN_GOTO,
    TOKEN_VAR_ARG,
    TOKEN_LENGTH,
    
    TOKEN_BIT_NOT = 50,
    TOKEN_BIT_OR = 51,
    TOKEN_BIT_AND = 52,
    TOKEN_BIT_SHIFT_LEFT = 53,
    TOKEN_BIT_SHIFT_RIGHT = 54,
    
    TOKEN_EQUALS = 60,
    TOKEN_NOT_EQUALS = 61,
    TOKEN_GREATER_THAN = 62,
    TOKEN_GREATER_THAN_OR_EQUAL = 63,
    TOKEN_LESS_THAN = 64,
    TOKEN_LESS_THAN_OR_EQUAL = 65,
    
    TOKEN_STRING_CONCAT = 70,
    
    TOKEN_PLUS = 101,
    TOKEN_MINUS = 102,
    TOKEN_MULTIPLY = 103,
    TOKEN_DIVIDE = 104,
    TOKEN_POWER = 105,
    TOKEN_MODULO = 106,
} TokenType;

const char *token_type_name(TokenType type);

typedef struct Token {
    Position position;
    TokenType type;
    const char *text;
    size_t text_len;
} Token;

bool token_is_keyword(const char *keyword, Token *token);

#endif //LUA_PARSER_TOKEN_H
