#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "interpreter.h"

#define PERFORM_BINOP(op) \
    switch (lhs.type) { \
        case OBJ_INT: {\
            int64_t left = lhs.as.integer;\
            int64_t right = rhs.as.integer;\
            return (Object) {\
                .type = lhs.type,\
                .as.integer = left op right,\
            };\
        }\
        case OBJ_FLOAT: {\
            double left = lhs.as.floating;\
            double right = rhs.as.floating;\
            return (Object) {\
                .type = lhs.type,\
                .as.floating= left op right,\
            };\
        }\
    }\

static Object execute_funcall(Interpreter* interpreter, FunctionCall* funcall, Scope* parent_scope) {
    FunctionDeclaration* fun = interpreter_find_fundecl(interpreter, funcall->id);
    if (!fun) {
        error_and_die("no such function: "SPAN_FMT, SPAN_ARG(funcall->id));
    }

    if (funcall->args_size != fun->args_size) {
        error_and_die(SPAN_FMT" expected: %d arguments but got: %d", SPAN_ARG(fun->id), fun->args_size, funcall->args_size);
    }

    Scope* scope = scope_make();

    for (int i = 0; i < fun->args_size; i++) {
        Object object = execute_expression(interpreter, funcall->args[i], parent_scope);

        scope_append_variable(scope, (Variable) {
            .id = fun->args[i],
            .object = object,
        });
    }

    Object result = execute_function_declaration(interpreter, fun, scope);

    scope_free(scope);

    return result;
}

static Object execute_primary(Interpreter* interpreter, Value* value, Scope* scope) {
    switch (value->type) {
        case VAL_INT:
            return (Object) {
                .type = OBJ_INT,
                .as.integer = value->as.integer,
            };
        case VAL_FLOAT:
            return (Object) {
                .type = OBJ_FLOAT,
                .as.floating = value->as.floating,
            };
        case VAL_IDENT: {
            Variable* variable = scope_find_variable(scope, value->as.identifier);

            if (!variable) {
                error_and_die("no such variable: "SPAN_FMT, SPAN_ARG(value->as.identifier));
            }

            return variable->object;
        case VAL_FUNCALL:
            return execute_funcall(interpreter, &value->as.funcall, scope);
            break;
        }
    }
}

static Object execute_binary(Interpreter* interpreter, BinaryExpression* binary, Scope* scope) {
    switch (binary->type) {
        case BIN_ADD: {
            Object lhs = execute_expression(interpreter, binary->lhs, scope);
            Object rhs = execute_expression(interpreter, binary->rhs, scope);

            PERFORM_BINOP(+);
            break;
        }
        case BIN_SUB: {
            Object lhs = execute_expression(interpreter, binary->lhs, scope);
            Object rhs = execute_expression(interpreter, binary->rhs, scope);

            PERFORM_BINOP(-);
            break;
        }
        case BIN_MUL: {
            Object lhs = execute_expression(interpreter, binary->lhs, scope);
            Object rhs = execute_expression(interpreter, binary->rhs, scope);

            PERFORM_BINOP(*);
            break;
        }
        case BIN_DIV: {
            Object lhs = execute_expression(interpreter, binary->lhs, scope);
            Object rhs = execute_expression(interpreter, binary->rhs, scope);

            PERFORM_BINOP(/);
            break;
        }
    }
}

Scope* scope_make() {
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;

    scope->children = NULL;
    scope->children_size = 0;
    scope->children_cap = 0;

    scope->variables = NULL;
    scope->variables_size = 0;
    scope->variables_cap = 0;

    return scope;
}

void scope_free(Scope* scope) {
    if (scope->children) {
        for (int i = 0; i < scope->children_size; i++) {
            scope_free(scope->children[i]);
        }

        free(scope->children);
    }

    free(scope->variables);
    free(scope);
}

void scope_append_child(Scope* scope, Scope* child) {
    assert(scope != NULL);
    assert(child != NULL);

    child->parent = scope;

    if (!scope->children) {
        scope->children_cap = 1;
        scope->children = malloc(sizeof(Scope*));
    } else {
        scope->children_cap++;
        scope->children = realloc(scope->children, sizeof(Scope*) * scope->children_cap);
    }

    scope->children[scope->children_size++] = child;
}

void scope_append_variable(Scope* scope, Variable variable) {
    assert(scope != NULL);

    if (!scope->variables) {
        scope->variables_cap = 1;
        scope->variables = malloc(sizeof(Variable));
    } else {
        scope->variables_cap++;
        scope->variables = realloc(scope->variables, sizeof(Variable) * scope->variables_cap);
    }

    scope->variables[scope->variables_size++] = variable;
}

Variable* scope_find_variable(Scope* scope, Span id) {
    Variable* variable = NULL;

    for (int i = 0; i < scope->variables_size; i++) {
        if (span_equals(scope->variables[i].id, id)) {
            variable = &scope->variables[i];
        }
    }

    return variable;
}

void interpreter_init(Interpreter* interpreter, Module* module) {
    assert(interpreter != NULL);
    assert(module != NULL);

    interpreter->module = module;
}

void interpreter_deinit(Interpreter* interpreter) {
    assert(interpreter != NULL);

    module_free(interpreter->module);
}

FunctionDeclaration* interpreter_find_fundecl(Interpreter* interpreter, Span id) {
    Module* module = interpreter->module;

    FunctionDeclaration* fundecl = NULL;
    for (int i = 0; i < module->fundecls_size; i++) {
        if (span_equals(module->fundecls[i].id, id)) {
            fundecl = &module->fundecls[i];
        }
    }

    return fundecl;
}

Object execute_expression(Interpreter* interpreter, Expression* expression, Scope* scope) {
    switch (expression->type) {
        case EXPR_PRIMARY:
            return execute_primary(interpreter, &expression->as.primary, scope);
        case EXPR_BINARY:
            return execute_binary(interpreter, &expression->as.binary, scope);
            break;
    }
}

void execute_assignment(Interpreter* interpreter, Assignment* assignment, Scope* scope) {
    Variable* variable = scope_find_variable(scope, assignment->id);
    if (!variable) {
        error_and_die("no such variable: "SPAN_FMT, SPAN_ARG(assignment->id));
    }

    variable->object = execute_expression(interpreter, assignment->expr, scope);
}

void execute_let_block(Interpreter* interpreter, LetBlock* letblock, Scope* scope) {
    for (int i = 0; i < letblock->ids_size; i++) {
        scope_append_variable(scope, (Variable) {
            .id = letblock->ids[i],
        });
    }

    for (int i = 0; i < letblock->assignments_size; i++) {
        execute_assignment(interpreter, &letblock->assignments[i], scope);
    }
}

Object execute_block(Interpreter* interpreter, Block* block, Scope* scope) {
    if (block->children_size < 1) {
        error_and_die("any function is expected to return something");
    }

    if (block->children[block->children_size - 1].type != STMT_EXPRESSION) {
        error_and_die("expected return value");
    }

    for (int i = 0; i < block->children_size - 1; i++) {
        Statement* statement = &block->children[i];

        switch (statement->type) {
            case STMT_LETBLOCK:
                execute_let_block(interpreter, &statement->as.letblock, scope);
                break;
            case STMT_EXPRESSION: {
                (void) execute_expression(interpreter, statement->as.expression, scope);
                break;
            }
        }
    }

    return execute_expression(interpreter, block->children[block->children_size - 1].as.expression, scope);
}

Object execute_function_declaration(Interpreter* interpreter, FunctionDeclaration* fundecl, Scope* scope) {
    Object result = execute_block(interpreter, fundecl->block, scope);
    return result;
}

Object execute_module(Interpreter* interpreter) {
    Module* module = interpreter->module;
    FunctionDeclaration* entry_point = NULL;

    for (int i = 0; i < module->fundecls_size; i++) {
        FunctionDeclaration* fundecl = &module->fundecls[i];
        if (span_equals(fundecl->id, span_from_cstr("main"))) {
            entry_point = fundecl;
        }
    }

    if (!entry_point) {
        error_and_die("no entry main point function");
    }

    Scope* scope = scope_make();
    Object return_value = execute_function_declaration(interpreter, entry_point, scope);
    scope_free(scope);

    if (return_value.type != OBJ_INT) {
        error_and_die("main function should return integer");
    }

    return return_value;
}
