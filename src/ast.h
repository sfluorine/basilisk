#pragma once

#include <stdint.h>

#include "span.h"

typedef struct Expression_t Expression;

typedef struct {
    Span id;

    Expression** args;
    int args_size;
    int args_cap;
} FunctionCall;

void function_call_free(FunctionCall* funcall);

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_IDENT,
    VAL_FUNCALL,
} ValueType;

typedef struct {
    ValueType type;

    union {
        int64_t integer;
        double floating;
        Span identifier;
        FunctionCall funcall;
    } as;
} Value;

typedef enum {
    BIN_ADD,
    BIN_SUB,
    BIN_MUL,
    BIN_DIV,
    BIN_EQU,
    BIN_NEQU,
    BIN_GT,
    BIN_LT,
    BIN_GTEQ,
    BIN_LTEQ,
    BIN_AND,
    BIN_OR,
} BinaryExpressionType;

typedef struct {
    BinaryExpressionType type;

    Expression* lhs;
    Expression* rhs;
} BinaryExpression;

BinaryExpression binary_expression_make(BinaryExpressionType type, Expression* lhs, Expression* rhs);
void binary_expression_free(BinaryExpression* binary);

typedef enum {
    EXPR_PRIMARY,
    EXPR_BINARY,
} ExpressionType;

struct Expression_t {
    ExpressionType type;

    union {
        Value primary;
        BinaryExpression binary;
    } as;
};

Expression* expression_make();
void expression_free(Expression* expr);

typedef struct {
    Span id;
    Expression* expr;
} Assignment;

void assignment_free(Assignment* assignment);

typedef struct {
    Span* ids;
    int ids_size;
    int ids_cap;

    Assignment* assignments;
    int assignments_size;
    int assignments_cap;
} LetBlock;

void let_block_free(LetBlock* letblock);

typedef struct Statement_t Statement;

typedef struct Block_t {
    Statement* children;
    int children_size;
    int children_cap;
} Block;

Block* block_make();
void block_free(Block* block);

typedef struct {
    Expression* expr;

    Block* true_block;
    Block* false_block;
} IfStatement;

void if_statement_free(IfStatement* ifstatement);

typedef enum {
    STMT_LETBLOCK,
    STMT_IF,
    STMT_EXPRESSION,
} StatementType;

struct Statement_t {
    StatementType type;

    union {
        LetBlock letblock;
        IfStatement ifstatement;
        Expression* expression;
    } as;
};

void statement_free(Statement* statement);

typedef struct {
    Span id;

    Span* args;
    int args_size;
    int args_cap;

    Block* block;
} FunctionDeclaration;

void function_declaration_free(FunctionDeclaration* fundecl);

typedef struct {
    FunctionDeclaration* fundecls;
    int fundecls_size;
    int fundecls_cap;
} Module;

void module_free(Module* module);
