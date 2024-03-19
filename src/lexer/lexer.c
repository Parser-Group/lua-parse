#include "lexer.h"

#include <ctype.h>
#include <string.h>

#include "utils.h"
#include "char_position.h"

typedef struct {
    const char *text;
    TokenType type;
} LiteralToken;

LiteralToken literal_tokens[] = {
        {.text = "~", .type = TOKEN_BIT_NOT},
        {.text = "|", .type = TOKEN_BIT_OR},
        {.text = "&", .type = TOKEN_BIT_AND},
        {.text = "<<", .type = TOKEN_BIT_SHIFT_LEFT},
        {.text = ">>", .type = TOKEN_BIT_SHIFT_RIGHT},
        {.text = ">>", .type = TOKEN_BIT_SHIFT_RIGHT},

        {.text = "==", .type = TOKEN_EQUALS},
        {.text = "~=", .type = TOKEN_NOT_EQUALS},
        {.text = ">", .type = TOKEN_GREATER_THAN},
        {.text = ">=", .type = TOKEN_GREATER_THAN_OR_EQUAL},
        {.text = "<", .type = TOKEN_LESS_THAN},
        {.text = "<=", .type = TOKEN_LESS_THAN_OR_EQUAL},

        {.text = "::", .type = TOKEN_GOTO},
        {.text = "..", .type = TOKEN_STRING_CONCAT},
        {.text = "...", .type = TOKEN_VAR_ARG},

        {.text = "(", .type = TOKEN_OPEN_PAREN},
        {.text = ")", .type = TOKEN_CLOSE_PAREN},
        {.text = "{", .type = TOKEN_OPEN_BRACE},
        {.text = "}", .type = TOKEN_CLOSE_BRACE},
        {.text = "[", .type = TOKEN_OPEN_BRACKET},
        {.text = "]", .type = TOKEN_CLOSE_BRACKET},

        {.text = ".", .type = TOKEN_DOT},
        {.text = ":", .type = TOKEN_COLON},
        {.text = ";", .type = TOKEN_SEMICOLON},
        {.text = ",", .type = TOKEN_COMMA},


        {.text = "=", .type = TOKEN_EQUAL},
        {.text = "+", .type = TOKEN_PLUS},
        {.text = "-", .type = TOKEN_MINUS},
        {.text = "*", .type = TOKEN_TIMES},
        {.text = "/", .type = TOKEN_DIVIDE},
        {.text = "^", .type = TOKEN_POWER},
        {.text = "%", .type = TOKEN_MODULO},
        {.text = "#", .type = TOKEN_LENGTH},
};
#define literal_tokens_count sizeof(literal_tokens)/sizeof(literal_tokens[0])

const char *keywords[] = {
        "and", "break", "do", "else", "elseif", "end",
        "false", "for", "function", "goto", "if",
        "in", "local", "nil", "or", "repeat",
        "return", "then", "true", "until", "while"
};
#define keywords_count sizeof(keywords)/sizeof(keywords[0])

// TODO: clean up macros
#define peek(l, offset) (l->content[l->cursor + offset])
#define can_peek(l, offset) ((l->cursor + offset) < l->content_len)

Lexer lexer_new(const char *content, uSize content_len) {
    Lexer l = {0};
    l.content = content;
    l.content_len = content_len;
    return l;
}

CharPosition lexer_get_position(Lexer *l) {
    CharPosition pos;
    pos.line = l->line;
    pos.column = l->cursor - l->begin_of_line;
    return pos;
}

void lexer_consume(Lexer *l, uSize len) {
    char x = peek(l, 0);
    l->cursor += len;

    if (x == '\r') {
        if (peek(l, 0) == '\n') {
            l->cursor += 1;
        }

        l->line += 1;
        l->begin_of_line = l->cursor;
        return;
    }

    if (x == '\n') {
        l->line += 1;
        l->begin_of_line = l->cursor;
    }
}

bool lexer_starts_with(Lexer *l, const char *prefix) {
    uSize prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return true;
    }

    if (l->cursor + prefix_len - 1 >= l->content_len) {
        return false;
    }

    for (uSize i = 0; i < prefix_len; ++i) {
        if (prefix[i] != l->content[l->cursor + i]) {
            return false;
        }
    }

    return true;
}

void lexer_trim_left(Lexer *l) {
    while (can_peek(l, 0) && isspace(peek(l, 0))) {
        UNUSED(lexer_consume(l, 1));
    }
}

bool is_symbol_start(char x) {
    return isalpha(x) || x == '_';
}

bool is_symbol(char x) {
    return isalnum(x) || x == '_';
}

Token lexer_next(Lexer *l) {
    lexer_trim_left(l);
    Token token = {0};
    token.text = &peek(l, 0);
    CharPosition start_pos = lexer_get_position(l);

    if (l->cursor >= l->content_len) {
        return token;
    }

    if (is_symbol_start(peek(l, 0))) {
        token.type = TOKEN_SYMBOL;

        while (can_peek(l, 0) && is_symbol(peek(l, 0))) {
            lexer_consume(l, 1);
        }

        token.text_len = &peek(l, 0) - token.text;

        for (uSize i = 0; i < keywords_count; ++i) {
            uSize keyword_len = strlen(keywords[i]);
            if (keyword_len == token.text_len && memcmp(keywords[i], token.text, keyword_len) == 0) {
                token.type = TOKEN_KEYWORD;
                break;
            }
        }

        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }

    if (lexer_starts_with(l, "--")) {
        lexer_consume(l, 2);
        token.type = TOKEN_COMMENT;

        if (lexer_starts_with(l, "[[")) {
            lexer_consume(l, 2);

            while (!lexer_starts_with(l, "]]")) {
                lexer_consume(l, 1);
            }
        } else {
            while (can_peek(l, 0) && peek(l, 0) != '\n' && peek(l, 0) != '\r') {
                lexer_consume(l, 1);
            }
        }

        token.text_len = &peek(l, 0) - token.text;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }

    if (peek(l, 0) == '\"' || peek(l, 0) == '\'') {
        const char quoteType = peek(l, 0);
        lexer_consume(l, 1);

        // TODO: should also handle escape sequence like \n
        while (can_peek(l, 0) && peek(l, 0) != quoteType) {
            lexer_consume(l, 1);
        }
        if (can_peek(l, 0)) {
            lexer_consume(l, 1);
        }

        token.type = TOKEN_STRING;
        token.text_len = &peek(l, 0) - token.text;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }

    //TODO: handle multi line string "[==['str']==]" "[['str']]" 
//    if (peek(0) == '[' && can_peek(1) && (peek(1) == '[' || peek(1) == '=')) {
//        lexer_consume(l, 1);
//    }

    if (isdigit(peek(l, 0))) {
        token.type = TOKEN_NUMBER;

        while (can_peek(l, 0) && isdigit(peek(l, 0))) {
            lexer_consume(l, 1);
        }

        token.text_len = &peek(l, 0) - token.text;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }

    int lit_tokens_count = literal_tokens_count;
    for (int i = 0; i < lit_tokens_count; ++i) {
        //NOTE: this code assumes there no newline in literal_tokens[i].text
        if (lexer_starts_with(l, literal_tokens[i].text)) {
            uSize text_len = strlen(literal_tokens[i].text);
            lexer_consume(l, text_len);

            token.type = literal_tokens[i].type;
            token.text_len = text_len;
            token.position = char_position_from_to(start_pos, lexer_get_position(l));
            return token;
        }
    }

    lexer_consume(l, 1);
    token.type = TOKEN_INVALID;
    token.text_len = 1;
    token.position = char_position_to_position(lexer_get_position(l));
    return token;
}
