#include "output.h"
#include "common.h"

#define OUTPUT_METHOD(funcName, outputCode)                                         \
void funcName(Parser *p, Position *pos, const char *message, size_t message_len) {  \
    if (p->onOutput != NULL) {                                                      \
        p->onOutput(pos, outputCode, message, message_len);                         \
    }                                                                               \
}

OUTPUT_METHOD(output_miss_open_paren, OUTPUT_MISS_OPEN_PARENTHESIS)
OUTPUT_METHOD(output_miss_close_paren, OUTPUT_MISS_CLOSE_PARENTHESIS)
OUTPUT_METHOD(output_miss_open_bracket, OUTPUT_MISS_OPEN_BRACKET)
OUTPUT_METHOD(output_miss_close_bracket, OUTPUT_MISS_CLOSE_BRACKET)
OUTPUT_METHOD(output_miss_open_brace, OUTPUT_MISS_OPEN_BRACE)
OUTPUT_METHOD(output_miss_close_brace, OUTPUT_MISS_CLOSE_BRACE)

OUTPUT_METHOD(output_miss_symbol, OUTPUT_MISS_SYMBOL)
OUTPUT_METHOD(output_miss_expression, OUTPUT_MISS_EXPRESSION)
OUTPUT_METHOD(output_miss_end, OUTPUT_MISS_END)

OUTPUT_METHOD(output_unexpected_token, OUTPUT_UNEXPECTED_TOKEN)
OUTPUT_METHOD(output_unexpected_keyword, OUTPUT_UNEXPECTED_KEYWORD)

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

        case OUTPUT_MISS_SYMBOL:
            return "MISS_SYMBOL";
        case OUTPUT_MISS_EXPRESSION:
            return "MISS_EXPRESSION";
        case OUTPUT_MISS_END:
            return "MISS_END";

        case OUTPUT_UNEXPECTED_TOKEN:
            return "UNEXPECTED_TOKEN";
        case OUTPUT_UNEXPECTED_KEYWORD:
            return "UNEXPECTED_KEYWORD";

        default:
            UNIMPLEMENTED("outputCode_to_string");
    }
}
