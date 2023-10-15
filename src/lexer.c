#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "lexer.h"

typedef struct {
    Token* tokens;
    int tokens_size;
    int tokens_cap;
} Tokens;

static void tokens_init(Tokens* tokens) {
    assert(tokens != NULL);

    tokens->tokens = NULL;
    tokens->tokens_size = 0;
    tokens->tokens_cap = 0;
}

static void tokens_push(Tokens* tokens, Token token) {
    if (!tokens->tokens) {
        tokens->tokens_cap = 1;
        tokens->tokens = malloc(sizeof(Token));

        assert(tokens->tokens != NULL);
    } else {
        tokens->tokens_cap++;
        tokens->tokens = realloc(tokens->tokens, sizeof(Token) * tokens->tokens_cap);

        assert(tokens->tokens != NULL);
    }

    tokens->tokens[tokens->tokens_size++] = token;
}

static const char* s_source = NULL;
static int s_line = 1;
static int s_col = 1;
static bool s_init = false;

static void advance() {
    if (*s_source == '\n') {
        s_line++;
        s_col = 1;
    } else {
        s_col++;
    }

    s_source++;
}

void lexer_init(const char* source) {
    assert(source != NULL);

    s_source = source;
    s_line = 1;
    s_col = 1;

    s_init = true;
}

Token* lexer_lex(int* size) {
    assert(s_init);

    Tokens tokens;
    int tokens_size = 0;

    tokens_init(&tokens);

    while (*s_source) {
        while (*s_source && (isspace(*s_source) || *s_source == '#')) {
            while (*s_source && isspace(*s_source)) {
                advance();
            }

            if (*s_source && *s_source == '#') {
                do {
                    advance();
                } while (*s_source && *s_source != '\n');
            }
        }

        const char* start = s_source;
        int start_line = s_line;
        int start_col = s_col;

        if (!*s_source) {
            break;
        }

        switch (*s_source) {
            case '(':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_LPAREN, span_make(start, 1)));
                tokens_size++;
                continue;
            case ')':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_RPAREN, span_make(start, 1)));
                tokens_size++;
                continue;
            case '[':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_LSBRACE, span_make(start, 1)));
                tokens_size++;
                continue;
            case ']':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_RSBRACE, span_make(start, 1)));
                tokens_size++;
                continue;
            case '{':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_LCBRACE, span_make(start, 1)));
                tokens_size++;
                continue;
            case '}':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_RCBRACE, span_make(start, 1)));
                tokens_size++;
                continue;
            case ',':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_COMMA, span_make(start, 1)));
                tokens_size++;
                continue;
            case '=':
                advance();

                if (*s_source == '=') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_EQUALEQUAL, span_make(start, 1)));
                    tokens_size++;
                } else {
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_EQUAL, span_make(start, 1)));
                    tokens_size++;
                }
                continue;
            case '!':
                advance();

                if (*s_source == '=') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_NOTEQUAL, span_make(start, 1)));
                    tokens_size++;
                } else {
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_BANG, span_make(start, 1)));
                    tokens_size++;
                }

                continue;
            case '+':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_PLUS, span_make(start, 1)));
                tokens_size++;
                continue;
            case '-': {
                advance();

                if (isdigit(*s_source)) {
                    int len = 0;
                    do {
                        len++;
                        advance();
                    } while (*s_source && isdigit(*s_source));

                    if (*s_source == '.') {
                        len++;
                        advance();

                        int mantissa_len = 0;
                        do {
                            mantissa_len++;
                            advance();
                        } while (*s_source && isdigit(*s_source));

                        Span span = span_make(start, len + mantissa_len);

                        if (mantissa_len == 0) {
                            error_and_die("invalid floating point number: "SPAN_FMT, SPAN_ARG(span));
                        }

                        tokens_push(&tokens, token_make(start_line, start_col, TOK_FLOATLITERAL, span));
                        tokens_size++;
                    } else {
                        tokens_push(&tokens, token_make(start_line, start_col, TOK_INTLITERAL, span_make(start, len)));
                        tokens_size++;
                    }
                } else if (*s_source == '>') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_ARROW, span_make(start, 2)));
                    tokens_size++;
                } else {
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_MINUS, span_make(start, 1)));
                    tokens_size++;
                }

                continue;
            }
            case '*':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_STAR, span_make(start, 1)));
                tokens_size++;
                continue;
            case '/':
                advance();
                tokens_push(&tokens, token_make(start_line, start_col, TOK_SLASH, span_make(start, 1)));
                tokens_size++;
                continue;
            case '<':
                advance();
                if (*s_source == '=') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_LESSEQUAL, span_make(start, 2)));
                    tokens_size++;
                } else {
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_LESS, span_make(start, 2)));
                    tokens_size++;
                }
                continue;
            case '>':
                advance();

                if (*s_source == '=') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_GREATEREQUAL, span_make(start, 2)));
                    tokens_size++;
                } else {
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_GREATER, span_make(start, 2)));
                    tokens_size++;
                }

                continue;
            case '&':
                advance(); 

                if (*s_source == '&') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_ANDAND, span_make(start, 2)));
                    tokens_size++;
                } else {
                    error_and_die("invalid token: &");
                }

                continue;
            case '|':
                advance(); 

                if (*s_source == '|') {
                    advance();
                    tokens_push(&tokens, token_make(start_line, start_col, TOK_BARBAR, span_make(start, 2)));
                    tokens_size++;
                } else {
                    error_and_die("invalid token: |");
                }

                continue;
            default:
                break;
        }

        if (isdigit(*s_source)) {
            int len = 0;
            do {
                len++;
                advance();
            } while (*s_source && isdigit(*s_source));

            if (*s_source == '.') {
                len++;
                advance();

                int mantissa_len = 0;
                do {
                    mantissa_len++;
                    advance();
                } while (*s_source && isdigit(*s_source));

                Span span = span_make(start, len + mantissa_len);

                if (mantissa_len == 0) {
                    error_and_die("invalid floating point number: "SPAN_FMT, SPAN_ARG(span));
                }

                tokens_push(&tokens, token_make(start_line, start_col, TOK_FLOATLITERAL, span));
                tokens_size++;
            } else {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_INTLITERAL, span_make(start, len)));
                tokens_size++;
            }
        } else if (isalpha(*s_source) || *s_source == '_') {
            int len = 0;
            do {
                len++;
                advance();
            } while (*s_source && (isalnum(*s_source) || *s_source == '_'));

            Span span = span_make(start, len);

            if (span_equals(span, span_from_cstr("def"))) {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_DEF, span_make(start, len)));
                tokens_size++;
            } else if (span_equals(span, span_from_cstr("let"))) {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_LET, span_make(start, len)));
                tokens_size++;
            } else if (span_equals(span, span_from_cstr("if"))) {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_IF, span_make(start, len)));
                tokens_size++;
            } else if (span_equals(span, span_from_cstr("else"))) {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_ELSE, span_make(start, len)));
                tokens_size++;
            } else {
                tokens_push(&tokens, token_make(start_line, start_col, TOK_IDENTIFIER, span_make(start, len)));
                tokens_size++;
            }
        } else {

            int len = 0;
            do {
                len++;
                advance();
            } while (*s_source && !isspace(*s_source));

            Span span = span_make(start, len);

            fprintf(stderr, "[%d:%d] ", start_line, start_col);
            error_and_die("found garbage token: "SPAN_FMT, SPAN_ARG(span));
        }
    }

    *size = tokens_size;

    return tokens.tokens;
}
