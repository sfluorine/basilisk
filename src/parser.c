#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "parser.h"

static bool parser_eof(Parser* parser) {
    return parser->cursor >= parser->tokens_size;
}

static Token* current_token(Parser* parser) {
    if (parser_eof(parser))
        return NULL;

    return &parser->tokens[parser->cursor];
}

static bool expect(Parser* parser, TokenType type) {
    if (!current_token(parser))
        return false;

    return current_token(parser)->type == type;
}

static void advance(Parser* parser) {
    if (!parser_eof(parser))
        parser->cursor++;
}

static void match(Parser* parser, TokenType type) {
    if (!expect(parser, type)) {
        if (parser_eof(parser)) {
            error_and_die("unexpected end of file");
        } else {
            error_and_die("expected: %d but got: ", type, SPAN_FMT, SPAN_ARG(current_token(parser)->span));
        }
    }

    advance(parser);
}

static bool expect_comparison(Parser* parser) {
    if (!current_token(parser))
        return false;

    switch (current_token(parser)->type) {
        case TOK_EQUALEQUAL:
        case TOK_NOTEQUAL:
        case TOK_GREATEREQUAL:
        case TOK_LESSEQUAL:
        case TOK_GREATER:
        case TOK_LESS:
            return true;
        default:
            return false;
    }
}

void parser_init(Parser* parser, Token* tokens, int tokens_size) {
    parser->tokens = tokens;
    parser->tokens_size = tokens_size;
    parser->cursor = 0;
}

void parser_deinit(Parser* parser) {
    free(parser->tokens);
    parser->tokens = NULL;
    parser->tokens_size = 0;
    parser->cursor = 0;
}

Expression* parse_primary(Parser* parser) {
    if (parser_eof(parser)) {
        error_and_die("unexpected end of file");
    }

    if (expect(parser, TOK_LPAREN)) {
        advance(parser);

        Expression* expr = parse_expression(parser);

        match(parser, TOK_RPAREN);

        return expr;
    } else if (expect(parser, TOK_INTLITERAL)) {
        Value value = {
            .type = VAL_INT,
            .as.integer = strtoll(current_token(parser)->span.data, NULL, 10),
        };

        advance(parser);

        Expression* expr = expression_make();
        expr->type = EXPR_PRIMARY;
        expr->as.primary = value;

        return expr;
    } else if (expect(parser, TOK_FLOATLITERAL)) {
        Value value = {
            .type = VAL_FLOAT,
            .as.floating = strtod(current_token(parser)->span.data, NULL),
        };

        advance(parser);

        Expression* expr = expression_make();
        expr->type = EXPR_PRIMARY;
        expr->as.primary = value;

        return expr;
    } else if (expect(parser, TOK_IDENTIFIER)) {
        Token* id = current_token(parser);
        advance(parser);

        if (expect(parser, TOK_LSBRACE)) {
            advance(parser);

            Expression** args = NULL;
            int args_size = 0;
            int args_cap = 0;

            bool first = true;
            while (!parser_eof(parser) && !expect(parser, TOK_RSBRACE)) {
                if (!first) {
                    match(parser, TOK_COMMA);
                }

                Expression* expression = parse_expression(parser);

                if (!args) {
                    args_cap = 1;
                    args = malloc(sizeof(Expression*));
                } else {
                    args_cap++;
                    args = realloc(args, sizeof(Expression*) * args_cap);
                }

                args[args_size++] = expression;

                first = false;
            }

            match(parser, TOK_RSBRACE);

            FunctionCall funcall = {
                .id = id->span,
                .args = args,
                .args_size = args_size,
                .args_cap = args_cap,
            };

            Value value = {
                .type = VAL_FUNCALL,
                .as.funcall = funcall,
            };

            Expression* expr = expression_make();
            expr->type = EXPR_PRIMARY;
            expr->as.primary = value;

            return expr;
        } else {
            Value value = {
                .type = VAL_IDENT,
                .as.identifier = id->span,
            };

            Expression* expr = expression_make();
            expr->type = EXPR_PRIMARY;
            expr->as.primary = value;

            return expr;
        }
    }

    error_and_die("unexpected token: "SPAN_FMT, SPAN_ARG(current_token(parser)->span));
}

Expression* parse_factor(Parser* parser) {
    Expression* lhs = parse_primary(parser);

    while (expect(parser, TOK_STAR) || expect(parser, TOK_SLASH)) {
        BinaryExpressionType type = (current_token(parser)->type == TOK_STAR ? BIN_MUL : BIN_DIV);
        advance(parser);

        Expression* rhs = parse_primary(parser);

        Expression* binary = expression_make();
        binary->type = EXPR_BINARY;
        binary->as.binary = binary_expression_make(type, lhs, rhs);

        lhs = binary;
    }

    return lhs;
}

Expression* parse_term(Parser* parser) {
    Expression* lhs = parse_factor(parser);

    while (expect(parser, TOK_PLUS) || expect(parser, TOK_MINUS)) {
        BinaryExpressionType type = (current_token(parser)->type == TOK_PLUS ? BIN_ADD : BIN_SUB);
        advance(parser);

        Expression* rhs = parse_factor(parser);

        Expression* binary = expression_make();
        binary->type = EXPR_BINARY;
        binary->as.binary = binary_expression_make(type, lhs, rhs);

        lhs = binary;
    }

    return lhs;
}

Expression* parse_expression(Parser* parser) {
    Expression* lhs = parse_term(parser);

    while (expect_comparison(parser)) {
        BinaryExpressionType type;

        switch (current_token(parser)->type) {
            case TOK_EQUALEQUAL:
                type = BIN_EQU;
                break;
            case TOK_NOTEQUAL:
                type = BIN_NEQU;
                break;
            case TOK_GREATEREQUAL:
                type = BIN_GTEQ;
                break;
            case TOK_LESSEQUAL:
                type = BIN_LTEQ;
                break;
            case TOK_GREATER:
                type = BIN_GT;
                break;
            case TOK_LESS:
                type = BIN_LT;
                break;
            case TOK_ANDAND:
                type = BIN_AND;
                break;
            case TOK_BARBAR:
                type = BIN_OR;
                break;
            default:
                break;
        }

        advance(parser);

        Expression* rhs = parse_term(parser);

        Expression* binary = expression_make();
        binary->type = EXPR_BINARY;
        binary->as.binary = binary_expression_make(type, lhs, rhs);

        lhs = binary;
    }

    return lhs;
}

Assignment parse_assignment(Parser* parser) {
    Token* id = current_token(parser);
    match(parser, TOK_IDENTIFIER);

    match(parser, TOK_ARROW);

    Expression* expr = parse_expression(parser);

    return (Assignment) {
        .id = id->span,
        .expr = expr,
    };
}

Block* parse_block(Parser* parser) {
    match(parser, TOK_LCBRACE);

    Block* block = block_make();

    while (!parser_eof(parser) && !expect(parser, TOK_RCBRACE)) {
        Statement statement = parse_statement(parser);

        if (!block->children) {
            block->children_cap = 1;
            block->children = malloc(sizeof(Statement));
        } else {
            block->children_cap++;
            block->children = realloc(block->children, sizeof(Statement) * block->children_cap);
        }

        block->children[block->children_size++] = statement;
    }

    match(parser, TOK_RCBRACE);

    return block;
}

LetBlock parse_let_block(Parser* parser) {
    match(parser, TOK_LET);

    match(parser, TOK_LSBRACE);

    Span* ids = NULL;
    int ids_size = 0;
    int ids_cap = 0;

    bool first = true;
    while (!parser_eof(parser) && !expect(parser, TOK_RSBRACE)) {
        if (!first) {
            match(parser, TOK_COMMA);
        }

        Token* id = current_token(parser);
        match(parser, TOK_IDENTIFIER);

        if (!ids) {
            ids_cap = 1;
            ids = malloc(sizeof(Span));
        } else {
            ids_cap++;
            ids = realloc(ids, sizeof(Span) * ids_cap);
        }

        ids[ids_size++] = id->span;

        first = false;
    }

    if (ids_size == 0) {
        error_and_die("let block expects atleast 1 identifier");
    }

    match(parser, TOK_RSBRACE);

    match(parser, TOK_ARROW);

    match(parser, TOK_LCBRACE);

    Assignment* assignments = NULL;
    int assignments_size = 0;
    int assignments_cap = 0;

    first = true;
    while (!parser_eof(parser) && !expect(parser, TOK_RCBRACE)) {
        if (!first) {
            match(parser, TOK_COMMA);
        }

        Assignment assignment = parse_assignment(parser);

        if (!ids) {
            assignments_cap = 1;
            assignments = malloc(sizeof(Assignment));
        } else {
            assignments_cap++;
            assignments = realloc(assignments, sizeof(Assignment) * assignments_cap);
        }

        assignments[assignments_size++] = assignment;

        first = false;
    }

    match(parser, TOK_RCBRACE);

    return (LetBlock) {
        .ids = ids,
        .ids_size = ids_size,
        .ids_cap = ids_cap,
        .assignments = assignments,
        .assignments_size = assignments_size,
        .assignments_cap = assignments_cap,
    };
}

IfStatement parse_if_statement(Parser* parser) {
    match(parser, TOK_IF);

    Expression* expr = parse_expression(parser);

    Block* true_block = parse_block(parser);

    match(parser, TOK_ELSE);

    Block* false_block = parse_block(parser);

    return (IfStatement) {
        .expr = expr,
        .true_block = true_block,
        .false_block = false_block,
    };
}

Statement parse_statement(Parser* parser) {
    if (expect(parser, TOK_LET)) {
        LetBlock letblock = parse_let_block(parser);
        return (Statement) {
            .type = STMT_LETBLOCK,
            .as.letblock = letblock,
        };
    } else if (expect(parser, TOK_IF)) {
        IfStatement ifstatement = parse_if_statement(parser);
        return (Statement) {
            .type = STMT_IF,
            .as.ifstatement = ifstatement,
        };
    } else {
        Expression* expression = parse_expression(parser);
        return (Statement) {
            .type = STMT_EXPRESSION,
            .as.expression = expression,
        };
    }
}

FunctionDeclaration parse_function_declaration(Parser* parser) {
    match(parser, TOK_DEF);

    Token* id = current_token(parser);
    match(parser, TOK_IDENTIFIER);

    match(parser, TOK_LSBRACE);

    Span* args = NULL;
    int args_size = 0;
    int args_cap = 0;

    bool first = true;
    while (!parser_eof(parser) && !expect(parser, TOK_RSBRACE)) {
        if (!first) {
            match(parser, TOK_COMMA);
        }

        Token* arg = current_token(parser);
        match(parser, TOK_IDENTIFIER);

        if (!args) {
            args_cap = 1;
            args = malloc(sizeof(Span));
        } else {
            args_cap++;
            args = realloc(args, sizeof(Span) * args_cap);
        }

        args[args_size++] = arg->span;

        first = false;
    }

    match(parser, TOK_RSBRACE);

    match(parser, TOK_ARROW);

    Block* block = parse_block(parser);

    return (FunctionDeclaration) {
        .id = id->span,
        .args = args,
        .args_size = args_size,
        .args_cap = args_cap,
        .block = block,
    };
}

Module parse_module(Parser* parser) {
    FunctionDeclaration* fundecls = NULL;
    int fundecls_size = 0;
    int fundecls_cap = 0;

    while (!parser_eof(parser)) {
        FunctionDeclaration fundecl = parse_function_declaration(parser);

        if (!fundecls) {
            fundecls_cap = 1;
            fundecls = malloc(sizeof(FunctionDeclaration));
        } else {
            fundecls_cap++;
            fundecls = realloc(fundecls, sizeof(FunctionDeclaration) * fundecls_cap);
        }

        fundecls[fundecls_size++] = fundecl;
    }

    return (Module) {
        .fundecls = fundecls,
        .fundecls_size = fundecls_size,
        .fundecls_cap = fundecls_cap,
    };
}
