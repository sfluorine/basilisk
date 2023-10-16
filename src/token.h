#pragma once

#include "span.h"

typedef enum {
    TOK_INTLITERAL,
    TOK_FLOATLITERAL,
    TOK_IDENTIFIER,

    TOK_RECORD,
    TOK_DEF,
    TOK_LET,
    TOK_IF,
    TOK_ELSE,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LSBRACE,
    TOK_RSBRACE,
    TOK_LCBRACE,
    TOK_RCBRACE,
    TOK_COMMA,
    TOK_EQUAL,
    TOK_BANG,
    TOK_DOT,

    TOK_ANDAND, // &&
    TOK_BARBAR, // ||

    TOK_ARROW,
    TOK_EQUALEQUAL,
    TOK_NOTEQUAL,
    TOK_GREATEREQUAL,
    TOK_LESSEQUAL,
    TOK_GREATER,
    TOK_LESS,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
} TokenType;

typedef struct {
    int line;
    int col;
    TokenType type;
    Span span;
} Token;

Token token_make(int line, int col, TokenType type, Span span);
