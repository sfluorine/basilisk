#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

int main() {
    lexer_init("def add[x, y] -> { let [z] -> { z -> x + y } z } def main[] -> { let [x] -> { x -> add[34, 35] } }");

    int tokens_size = 0;
    Token* tokens = lexer_lex(&tokens_size);

    Parser parser;
    parser_init(&parser, tokens, tokens_size);

    Module module = parse_module(&parser);
    module_free(&module);

    parser_deinit(&parser);
}
