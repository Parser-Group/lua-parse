#include "parser.h"

Parser parser_new(Lexer *l) {
    Parser parser;
    parser.lexer = l;
    return parser;
}

void parser_next() {
    
}