#include <string.h>
#include "parser_utils.h"

bool keyword_is(const char* keyword, Token token) {
    if (strlen(keyword) > token.text_len) {
        return false;
    }

    return token.type == TOKEN_KEYWORD
           && memcmp(keyword, token.text, token.text_len) == 0;
}
