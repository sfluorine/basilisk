#pragma once

#include "token.h"

void lexer_init(const char* source);

Token* lexer_lex(int* size);
