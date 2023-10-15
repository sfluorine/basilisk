#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

int main() {
    lexer_init("def add[x,y] -> { x + y } def main[] -> { let [x, y, z] -> { x -> 3, y -> 2, z -> add[x * x, y] } let [a] -> { a -> x + y + z } x * x * x if (x == 4) { 3 } else { y } a }");

    int tokens_size = 0;
    Token* tokens = lexer_lex(&tokens_size);

    Parser parser;
    parser_init(&parser, tokens, tokens_size);
    Module module = parse_module(&parser);

    Interpreter interpreter;
    interpreter_init(&interpreter, &module);

    int return_value = execute_module(&interpreter).as.integer;

    interpreter_deinit(&interpreter);

    parser_deinit(&parser);

    return return_value;
}
