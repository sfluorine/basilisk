#pragma once

#include "span.h"

typedef enum {
    TOK_INTLITERAL,
    TOK_FLOATLITERAL,
    TOK_IDENTIFIER,

    TOK_DEF,
    TOK_LET,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LSBRACE,
    TOK_RSBRACE,
    TOK_LCBRACE,
    TOK_RCBRACE,
    TOK_COMMA,
    TOK_EQUAL,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,

    TOK_ARROW
} TokenType;

typedef struct {
    int line;
    int col;
    TokenType type;
    Span span;
} Token;

Token token_make(int line, int col, TokenType type, Span span);
