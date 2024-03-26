#include "output.h"
#include "common.h"

void output_miss_open_paren(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_OPEN_PARENTHESIS, message, message_len);
}
void output_miss_close_paren(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_CLOSE_PARENTHESIS, message, message_len);
}
void output_miss_open_bracket(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_OPEN_BRACKET, message, message_len);
}
void output_miss_close_bracket(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_CLOSE_BRACKET, message, message_len);
}
void output_miss_open_brace(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_OPEN_BRACE, message, message_len);
}
void output_miss_close_brace(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_CLOSE_BRACE, message, message_len);
}

void output_miss_symbol(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_SYMBOL, message, message_len);
}
void output_miss_expression(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_MISS_EXPRESSION, message, message_len);
}

void output_unexpected_keyword(Parser *p, Position *pos, const char *message, size_t message_len) {
    p->onOutput(pos, OUTPUT_UNEXPECTED_KEYWORD, message, message_len);
}
