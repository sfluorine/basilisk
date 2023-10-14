#include "token.h"

Token token_make(int line, int col, TokenType type, Span span) {
    return (Token) {
        .line = line,
        .col = col,
        .type = type,
        .span = span,
    };
}
