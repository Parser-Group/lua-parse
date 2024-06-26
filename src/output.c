#include "output.h"
#include "common.h"

#define OUTPUT_METHOD_IMPL(funcName, outputCode)                                    \
void funcName(Parser *p, Position *pos, const char *message, size_t message_len) {  \
    if (p->onOutput == NULL) {                                                      \
        return;                                                                     \
    }                                                                               \
                                                                                    \
    p->onOutput(pos, outputCode, message, message_len);                             \
}

OUTPUT_METHOD_IMPL(output_miss_open_paren, OUTPUT_MISS_OPEN_PARENTHESIS)
OUTPUT_METHOD_IMPL(output_miss_close_paren, OUTPUT_MISS_CLOSE_PARENTHESIS)
OUTPUT_METHOD_IMPL(output_miss_open_bracket, OUTPUT_MISS_OPEN_BRACKET)
OUTPUT_METHOD_IMPL(output_miss_close_bracket, OUTPUT_MISS_CLOSE_BRACKET)
OUTPUT_METHOD_IMPL(output_miss_open_brace, OUTPUT_MISS_OPEN_BRACE)
OUTPUT_METHOD_IMPL(output_miss_close_brace, OUTPUT_MISS_CLOSE_BRACE)

OUTPUT_METHOD_IMPL(output_miss_comma, OUTPUT_MISS_COMMA)
OUTPUT_METHOD_IMPL(output_miss_equal, OUTPUT_MISS_EQUAL)

OUTPUT_METHOD_IMPL(output_miss_symbol, OUTPUT_MISS_SYMBOL)
OUTPUT_METHOD_IMPL(output_miss_expression, OUTPUT_MISS_EXPRESSION)
OUTPUT_METHOD_IMPL(output_miss_keyword_end, OUTPUT_MISS_KEYWORD_END)
OUTPUT_METHOD_IMPL(output_miss_keyword_then, OUTPUT_MISS_KEYWORD_THEN)
OUTPUT_METHOD_IMPL(output_miss_keyword_do, OUTPUT_MISS_KEYWORD_DO)
OUTPUT_METHOD_IMPL(output_miss_keyword_in, OUTPUT_MISS_KEYWORD_IN)

OUTPUT_METHOD_IMPL(output_miss_goto_point, OUTPUT_MISS_GOTO_POINT)

void output_unexpected_token(Parser *p, Token *token, const char *message) {
    if (p->onOutput == NULL) {
        return;
    }
    
    char *token_str = token_to_string(&p->cur_token);
    int size = snprintf(NULL, 0, message, token_str) + 1;
    char *newMessage = malloc(sizeof(size));
    if ((newMessage) == NULL) {
        printf("%s:%d: Memory Allocation failed\n", __FILE__, __LINE__);
        exit(1);
    };
    
    sprintf(newMessage, message, token_str);
    free(token_str);
    
    p->onOutput(&token->position, OUTPUT_UNEXPECTED_TOKEN, newMessage, size);
    free(newMessage);
}
OUTPUT_METHOD_IMPL(output_unexpected_keyword, OUTPUT_UNEXPECTED_KEYWORD)
OUTPUT_METHOD_IMPL(output_unexpected_expression, OUTPUT_UNEXPECTED_EXPRESSION)

const char* outputCode_to_string(OutputCode code) {
    switch (code) {
        case OUTPUT_NONE:
            return "none";

        case OUTPUT_MISS_OPEN_PARENTHESIS:
            return "MISS_OPEN_PARENTHESIS";
        case OUTPUT_MISS_CLOSE_PARENTHESIS:
            return "MISS_CLOSE_PARENTHESIS";
        case OUTPUT_MISS_OPEN_BRACKET:
            return "MISS_OPEN_BRACKET";
        case OUTPUT_MISS_CLOSE_BRACKET:
            return "MISS_CLOSE_BRACKET";
        case OUTPUT_MISS_OPEN_BRACE:
            return "MISS_OPEN_BRACE";
        case OUTPUT_MISS_CLOSE_BRACE:
            return "MISS_CLOSE_BRACE";

        case OUTPUT_MISS_COMMA:
            return "MISS_COMMA";
        case OUTPUT_MISS_EQUAL:
            return "MISS_EQUAL";
            
        case OUTPUT_MISS_SYMBOL:
            return "MISS_SYMBOL";
        case OUTPUT_MISS_EXPRESSION:
            return "MISS_EXPRESSION";
        case OUTPUT_MISS_KEYWORD_END:
            return "MISS_KEYWORD_END";
        case OUTPUT_MISS_KEYWORD_THEN:
            return "MISS_KEYWORD_THEN";
        case OUTPUT_MISS_KEYWORD_DO:
            return "MISS_KEYWORD_DO";
        case OUTPUT_MISS_KEYWORD_IN:
            return "MISS_KEYWORD_IN";

        case OUTPUT_UNEXPECTED_TOKEN:
            return "UNEXPECTED_TOKEN";
        case OUTPUT_UNEXPECTED_KEYWORD:
            return "UNEXPECTED_KEYWORD";
        case OUTPUT_UNEXPECTED_EXPRESSION:
            return "UNEXPECTED_EXPRESSION";

        case OUTPUT_MISS_GOTO_POINT:
            return "MISS_GOTO_POINT";
            
        default:
            UNREACHABLE("outputCode_to_string");
    }
}
