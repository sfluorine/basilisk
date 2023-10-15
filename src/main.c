#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

static char* slurp_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        error_and_die("cannot open: %s", filepath);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(sizeof(char) * size + 1);
    buffer[size] = 0;

    fread(buffer, sizeof(char), size, file);
    fclose(file);

    return buffer;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        error_and_die("no input file provided");
    }

    char* input_buffer = slurp_file(argv[1]);
    lexer_init(input_buffer);

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

    free(input_buffer);

    return return_value;
}
