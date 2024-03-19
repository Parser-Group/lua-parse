#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "lexer/lexer.h"

int main() {
    char *content;
    uSize content_len;
    { // load from file
        FILE *file = fopen(".\\..\\Test.lua", "rb");
        if (file == NULL) {
            printf("Error opening file.\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);

        content_len = file_size;
        content = (char *) malloc(content_len);
        if (content == NULL) {
            printf("Memory allocation failed.\n");
            fclose(file);
            return 1;
        }

        fread(content, 1, file_size, file);
        content[file_size] = '\0';
        fclose(file);
    }

//    {
//        content = "local test = true\n"
//                "function test(message)\n"
//                "    print(message)\n"
//                "end\n"
//                "test(123)\n";
//        content_len = strlen(content);
//    }

    Lexer l = lexer_new(content, content_len);
    Token t;
    t = lexer_next(&l);

    while (t.type != TOKEN_END) {
        printf("'%.*s' (%s)\n", (int) t.text_len, t.text, token_type_name(t.type));

        t = lexer_next(&l);
    }

    return 0;
}
