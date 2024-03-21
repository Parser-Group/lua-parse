#include "lexer.h"

#include <ctype.h>
#include <string.h>

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
#define get_literal_tokens_count sizeof(literal_tokens)/sizeof(literal_tokens[0])

const char *keywords[] = {
        "and", "break", "do", "else", "elseif", "end",
        "false", "for", "function", "goto", "if",
        "in", "local", "nil", "or", "repeat",
        "return", "then", "true", "until", "while"
};
#define get_keywords_count sizeof(keywords)/sizeof(keywords[0])

Lexer lexer_new(const char *content, size_t content_len) {
    Lexer l = {0};
    l.content = content;
    l.content_len = content_len;
    return l;
}

CharPosition lexer_get_position(Lexer *l) {
    CharPosition pos;
    // NOTE: adding one for offset since first char in file begins at line 1
    pos.line = l->line + 1;
    pos.column = l->cursor - l->begin_of_line;
    return pos;
}

#define lexer_peek(l) (l->content[l->cursor])
#define lexer_peek_with_offset(l, offset) (l->content[l->cursor + offset])
#define lexer_can_peek(l) (l->cursor < l->content_len)
#define lexer_can_peek_with_offset(l, offset) (l->cursor + offset < l->content_len)

bool lexer_peek_is(Lexer *l, char c) {
    if (l->cursor >= l->content_len) {
        return false;
    }

    if (l->content[l->cursor] != c) {
        return false;
    }

    return true;
}

bool lexer_peek_is_with_offset(Lexer *l, char x, size_t offset) {
    if (l->cursor + offset >= l->content_len) {
        return false;
    }

    if (l->content[l->cursor + offset] != x) {
        return false;
    }

    return true;
}

void lexer_consume(Lexer *l, size_t len) {
    char x = l->content[l->cursor];
    l->cursor += len;

    if (x == '\r') {
        if (l->content[l->cursor] == '\n') {
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
    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return true;
    }

    if (l->cursor + prefix_len - 1 >= l->content_len) {
        return false;
    }

    for (size_t i = 0; i < prefix_len; ++i) {
        if (prefix[i] != l->content[l->cursor + i]) {
            return false;
        }
    }

    return true;
}

void lexer_trim_left(Lexer *l) {
    while (l->cursor < l->content_len && isspace(l->content[l->cursor])) {
        lexer_consume(l, 1);
    }
}

Token* multiline_string(Lexer *l, CharPosition *start_pos, Token *token, size_t count) {
    lexer_consume(l, count + 2);

    if (lexer_peek_is(l, ']')) {
        size_t found_count = 0;
        for (size_t i = 0; i < count; ++i) {
            if (!lexer_peek_is_with_offset(l, '=', 1 + i)) {
                break;
            }
                
            ++found_count;
        }

        if (found_count == count && lexer_peek_is_with_offset(l, ']', 1 + found_count)) {
            lexer_consume(l, 2 + found_count);
            return token;
        }
    }
            
    token->type = TOKEN_STRING;
    token->text = &l->content[l->cursor];
    
    while (true) {
        if (lexer_peek_is(l, ']')) {
            size_t foundCount = 0;
            for (size_t i = 0; i < count; ++i) {
                if (!lexer_peek_is_with_offset(l, '=', 1 + i)) {
                    break;
                }

                ++foundCount;
            }

            if (foundCount == count && lexer_peek_is_with_offset(l, ']', 1 + foundCount)) {
                token->text_len = &lexer_peek(l) - token->text;
                lexer_consume(l, 2 + foundCount);
                break;
            }
        }

        lexer_consume(l, 1);
    }
    
    token->position = char_position_from_to(*start_pos, lexer_get_position(l));
    return token;
}

Token lexer_next(Lexer *l) {
    lexer_trim_left(l);
    Token token = {0};
    token.text = &l->content[l->cursor];
    CharPosition start_pos = lexer_get_position(l);
    //NOTE: correcting column (not good design)
    ++start_pos.column;
    
    if (!lexer_can_peek(l)) {
        return token;
    }
    
    if (isalpha(lexer_peek(l)) || lexer_peek(l) == '_') {
        token.type = TOKEN_SYMBOL;
        
        while (lexer_can_peek(l) && (isalnum(lexer_peek(l)) || lexer_peek(l) == '_')) {
            lexer_consume(l, 1);
        }
 
        token.text_len = &lexer_peek(l) - token.text;

        size_t keywords_count = get_keywords_count;
        for (size_t i = 0; i < keywords_count; ++i) {
            size_t keyword_len = strlen(keywords[i]);
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
        
        if (lexer_starts_with(l, "[[")) {
            lexer_consume(l, 2);
            
            while (!lexer_starts_with(l, "]]")) {
                lexer_consume(l, 1);
            }
        } else {
            while (lexer_can_peek(l) && !lexer_peek_is(l, '\n') && !lexer_peek_is(l, '\r')) {
                lexer_consume(l, 1);
            }
        }
        
        token.type = TOKEN_COMMENT;
        token.text_len = &lexer_peek(l) - token.text;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }
    
    if (lexer_peek_is(l, '\"') || lexer_peek_is(l, '\'')) {
        //TODO: handle escape sequences
        const char quoteType = lexer_peek(l);
        lexer_consume(l, 1);
        
        token.text = &lexer_peek(l);
        while (lexer_can_peek(l) && !lexer_peek_is(l, quoteType)) {
            lexer_consume(l, 1);
        }
        token.text_len = &lexer_peek(l) - token.text;
        lexer_consume(l, 1);
        
        token.type = TOKEN_STRING;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }
    
    if (lexer_peek_is(l, '[')
        && (lexer_peek_is_with_offset(l, '[', 1)
            || lexer_peek_is_with_offset(l, '=', 1))) {
        
        size_t count = 0;
        while (lexer_peek_is_with_offset(l, '=', count + 1)) {
            ++count;
        }
        
        if (lexer_peek_is_with_offset(l, '[', count + 1)) {
            return *multiline_string(l, &start_pos, &token, count);
        }
    }
    
    if (isdigit(lexer_peek(l))) {
        lexer_consume(l, 1);
        
        if (lexer_peek_is(l, 'x') || lexer_peek_is(l, 'X')) {
            lexer_consume(l, 1);
            bool found_point = false;
            while (lexer_can_peek(l)) {
                if (!found_point && lexer_peek(l) == '.') {
                    lexer_consume(l, 1);
                    found_point = true;
                    continue;
                }
                
                if (!(isdigit(lexer_peek(l))
                    || lexer_peek(l) == 'a'
                    || lexer_peek(l) == 'b'
                    || lexer_peek(l) == 'c'
                    || lexer_peek(l) == 'd'
                    || lexer_peek(l) == 'e'
                    || lexer_peek(l) == 'f'
                    || lexer_peek(l) == 'A'
                    || lexer_peek(l) == 'B'
                    || lexer_peek(l) == 'C'
                    || lexer_peek(l) == 'D'
                    || lexer_peek(l) == 'E'
                    || lexer_peek(l) == 'F')) {
                    break;
                }
                lexer_consume(l, 1);
            }
            
            if (lexer_can_peek(l) && (lexer_peek(l) == 'p' || lexer_peek(l) == 'P')) {
                lexer_consume(l, 1);
                if (lexer_can_peek(l) && (lexer_peek(l) == '-' || lexer_peek(l) == '+')) {
                    lexer_consume(l, 1);
                }
                
                while (lexer_can_peek(l) && isdigit(lexer_peek(l))) {
                    lexer_consume(l, 1);
                }
            }
            
            token.type = TOKEN_NUMBER;
            token.text_len = &lexer_peek(l) - token.text;
            token.position = char_position_from_to(start_pos, lexer_get_position(l));
            return token;
        }
        
        bool found_point = false;
        bool found_exponent = false;
        while (lexer_can_peek(l)) {
            if (!found_point && !found_exponent && lexer_peek(l) == '.') {
                lexer_consume(l, 1);
                found_point = true;
                continue;
            }
            
            if (!found_exponent && (lexer_peek(l) == 'e' || lexer_peek(l) == 'E')) {
                lexer_consume(l, 1);
                found_exponent = true;
                
                if (lexer_can_peek(l) && (lexer_peek(l) == '-' || lexer_peek(l) == '+')) {
                    lexer_consume(l, 1);    
                }
                
                continue;
            }
            
            if (!isdigit(lexer_peek(l))) {
                break;
            }
            lexer_consume(l, 1);
        }
        
        token.type = TOKEN_NUMBER;
        token.text_len = &lexer_peek(l) - token.text;
        token.position = char_position_from_to(start_pos, lexer_get_position(l));
        return token;
    }
    
    int lit_tokens_count = get_literal_tokens_count;
    for (size_t i = 0; i < lit_tokens_count; ++i) {
        //NOTE: this code assumes there are no newline in liter_tokens[i].text
        if (lexer_starts_with(l, literal_tokens[i].text)) {
            size_t text_len = strlen(literal_tokens[i].text);
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