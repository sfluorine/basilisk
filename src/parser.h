#pragma once

#include "ast.h"
#include "token.h"

typedef struct {
    Token* tokens;
    int tokens_size;
    int cursor;
} Parser;

void parser_init(Parser* parser, Token* tokens, int tokens_size);
void parser_deinit(Parser* parser);

Expression* parse_primary(Parser* parser);
Expression* parse_factor(Parser* parser);
Expression* parse_term(Parser* parser);
Expression* parse_expression(Parser* parser);
Assignment parse_assignment(Parser* parser);
LetBlock parse_let_block(Parser* parser);
IfStatement parse_if_statement(Parser* parser);
Block* parse_block(Parser* parser);
Statement parse_statement(Parser* parser);
FunctionDeclaration parse_function_declaration(Parser* parser);
Record parse_record(Parser* parser);
RecordCreation parse_record_creation(Parser* parser);
Module parse_module(Parser* parser);
