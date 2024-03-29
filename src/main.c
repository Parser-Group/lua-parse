#include <stdio.h>
#include <malloc.h>
#include <time.h>

#include "lexer/lexer.h"
#include "parser/statement.h"

void printOutput(Position *pos, OutputCode code, const char *message, size_t message_len) {
//    char *positionStr = position_to_string(pos);
//    printf("%s -> %s: %.*s\n", positionStr, outputCode_to_string(code), (int)message_len, message);
//    free(positionStr);
}

int main() {
    char *content;
    size_t content_len;
    { // load from file
        FILE *file = fopen(".\\..\\Test.lua", "rb");
//        FILE *file = fopen("C:\\Coding\\C#\\Lua\\FINLuaDocumentationSumneko.lua", "rb");
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
    Parser p = parser_new(&l, printOutput);
    
    int start = clock();
    
    Statement *statement = parser_next(&p);
    while (statement->type != STATEMENT_END) {
//        printf("'%.*s' (%s)<%zu:%zu-%zu:%zu>\n",
//               (int)t.text_len,
//               t.text,
//               token_type_name(t.type),
//               t.position.start_line, t.position.start_column,
//               t.position.end_line, t.position.end_column);

        free(statement);
        statement = parser_next(&p);
    }

    int end = clock();

    printf("took %dms", end - start);
    
    return 0;
}
