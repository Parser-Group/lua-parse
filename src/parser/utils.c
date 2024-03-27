#include <string.h>
#include "utils.h"

bool token_is_keyword(const char* keyword, Token *token) {
    if (strlen(keyword) > token->text_len) {
        return false;
    }

    return token->type == TOKEN_KEYWORD
           && memcmp(keyword, token->text, token->text_len) == 0;
}
