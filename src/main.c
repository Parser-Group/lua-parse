#include <stdio.h>

#include "lexer/lexer.h"
#include "parser/statement.h"

#include <time.h>

void printOutput(Position *pos, OutputCode code, const char *message, size_t message_len) {
//    char *positionStr = position_to_string(pos);
//    printf("%s -> %s: %.*s\n", positionStr, outputCode_to_string(code), (int)message_len, message);
//    free(positionStr);
}

int main() {
    char *content;
    size_t content_len;
    { // load from file
//        FILE *file = fopen("./../Test.lua", "rb");
        
#if linux
        FILE *file = fopen("/mnt/c/Coding/C#/Lua/FINLuaDocumentationSumneko.lua", "rb"); // bigger file size: 500KB
#else
        FILE *file = fopen("C:\\Coding\\C#\\Lua\\FINLuaDocumentationSumneko.lua", "rb"); // bigger file size: 500KB
#endif

        if (file == NULL) {
            printf("Error opening file.\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);
        
        content_len = file_size + 1;
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
    
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    Lexer l = lexer_new(content, content_len);
    Parser p = parser_new(&l, printOutput);
    
    Statement statement = parser_next(&p);
    while (statement.type != STATEMENT_END) {
//        printf("'%.*s' (%s)<%zu:%zu-%zu:%zu>\n",
//               (int)t.text_len,
//               t.text,
//               token_type_name(t.type),
//               t.position.start_line, t.position.start_column,
//               t.position.end_line, t.position.end_column);

        statement_destroy(&statement);
        statement = parser_next(&p);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double time_diff_mqs = (double)(end_time.tv_nsec - start_time.tv_nsec) / 1000;
    printf("took %f ms", (time_diff_mqs / 1000));

    free(content);
    return 0;
}
