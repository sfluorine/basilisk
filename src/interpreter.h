#pragma once

#include "ast.h"

typedef struct Interpreter_t Interpreter;
typedef struct Scope_t Scope;

typedef void (*NativeFunction)(Interpreter* interpreter, Scope* scope);

typedef struct {
    NativeFunction fun;

    Expression* args;
    int args_size;
    int args_cap;
} Native;

typedef enum {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_VOID,
} ObjectType;

typedef struct {
    ObjectType type;

    union {
        int64_t integer;
        double floating;
    } as;
} Object;

typedef struct {
    Span id;
    Object object;
} Variable;

struct Scope_t {
    Scope** children;
    int children_size;
    int children_cap;

    Variable* variables;
    int variables_size;
    int variables_cap;
};

Scope* scope_make();
void scope_free(Scope* scope);

void scope_append_child(Scope* scope, Scope* child);
void scope_append_variable(Scope* scope, Variable variable);
Variable* scope_find_variable(Scope* scope, Span id);

struct Interpreter_t {
    Module* module;
};

void interpreter_init(Interpreter* interpreter, Module* module);
void interpreter_deinit(Interpreter* interpreter);

FunctionDeclaration* interpreter_find_fundecl(Interpreter* interpreter, Span id);

Object execute_expression(Interpreter* interpreter, Expression* expression, Scope* scope);
void execute_assignment(Interpreter* interpreter, Assignment* assignment, Scope* scope);
void execute_let_block(Interpreter* interpreter, LetBlock* letblock, Scope* scope);
Object execute_if_statement(Interpreter* interpreter, IfStatement* ifstatement, Scope* scope);
Object execute_block(Interpreter* interpreter, Block* block, Scope* scope);
Object execute_function_declaration(Interpreter* interpreter, FunctionDeclaration* fundecl, Scope* scope);
Object execute_module(Interpreter* interpreter);
