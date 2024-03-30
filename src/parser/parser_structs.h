#ifndef LUA_PARSER_PARSER_STRUCTS_H
#define LUA_PARSER_PARSER_STRUCTS_H

#include "lexer/lexer.h"

typedef enum {
    OUTPUT_NONE = 0,

    OUTPUT_MISS_OPEN_PARENTHESIS,
    OUTPUT_MISS_CLOSE_PARENTHESIS,
    OUTPUT_MISS_OPEN_BRACKET,
    OUTPUT_MISS_CLOSE_BRACKET,
    OUTPUT_MISS_OPEN_BRACE,
    OUTPUT_MISS_CLOSE_BRACE,

    OUTPUT_MISS_COMMA,
    OUTPUT_MISS_EQUAL,
    
    OUTPUT_MISS_SYMBOL,
    OUTPUT_MISS_EXPRESSION,
    OUTPUT_MISS_KEYWORD_END,
    OUTPUT_MISS_KEYWORD_THEN,
    OUTPUT_MISS_KEYWORD_DO,
    OUTPUT_MISS_KEYWORD_IN,
    
    OUTPUT_UNEXPECTED_TOKEN,
    OUTPUT_UNEXPECTED_KEYWORD,
    OUTPUT_UNEXPECTED_EXPRESSION,

    OUTPUT_MISS_GOTO_POINT,
    
} OutputCode;

const char* outputCode_to_string(OutputCode code);

typedef struct {
    Lexer *lexer;
    Token *cur_token;
    void (*onOutput) (Position *position, OutputCode code, const char *message, size_t message_len);
} Parser;

#endif //LUA_PARSER_PARSER_STRUCTS_H
