#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "common.h"

void function_call_free(FunctionCall* funcall) {
    assert(funcall != NULL);

    if (funcall->args) {
        for (int i = 0; i < funcall->args_size; i++) {
            expression_free(funcall->args[i]);
        }

        free(funcall->args);
    }
}

BinaryExpression binary_expression_make(BinaryExpressionType type, Expression* lhs, Expression* rhs) {
    return (BinaryExpression) {
        .type = type,
        .lhs = lhs,
        .rhs = rhs,
    };
}

void binary_expression_free(BinaryExpression* binary) {
    expression_free(binary->lhs);
    expression_free(binary->rhs);
}

Expression* expression_make() {
    Expression* expr = malloc(sizeof(Expression));
    if (!expr) {
        error_and_die("cannot allocate memory");
    }

    return expr;
}

void expression_free(Expression* expr) {
    assert(expr != NULL);

    switch (expr->type) {
        case EXPR_PRIMARY:
            if (expr->as.primary.type == VAL_FUNCALL) {
                function_call_free(&expr->as.primary.as.funcall);
            }

            free(expr);
            break;
        case EXPR_BINARY:
            binary_expression_free(&expr->as.binary);
            free(expr);
            break;
    }
}

void assignment_free(Assignment* assignment) {
    assert(assignment != NULL);

    expression_free(assignment->expr);
}

void let_block_free(LetBlock* letblock) {
    assert(letblock != NULL);

    if (letblock->ids) {
        free(letblock->ids);
    }

    if (letblock->assignments) {
        for (int i = 0; i < letblock->assignments_size; i++) {
            assignment_free(&letblock->assignments[i]);
        }

        free(letblock->assignments);
    }
}

Block* block_make() {
    Block* block = malloc(sizeof(Block));
    if (!block) {
        error_and_die("cannot allocate memory");
    }

    block->children = NULL;
    block->children_size = 0;
    block->children_cap = 0;

    return block;
}

void block_free(Block* block) {
    assert(block != NULL);

    if (block->children) {
        for (int i = 0; i < block->children_size; i++) {
            statement_free(&block->children[i]);
        }

        free(block->children);
    }

    free(block);
}

void if_statement_free(IfStatement* ifstatement) {
    assert(ifstatement != NULL);

    expression_free(ifstatement->expr);

    block_free(ifstatement->true_block);
    block_free(ifstatement->false_block);
}

void statement_free(Statement* statement) {
    assert(statement != NULL);

    switch (statement->type) {
        case STMT_LETBLOCK:
            let_block_free(&statement->as.letblock);
            break;
        case STMT_IF:
            if_statement_free(&statement->as.ifstatement);
            break;
        case STMT_EXPRESSION:
            expression_free(statement->as.expression);
            break;
    }
}

void function_declaration_free(FunctionDeclaration* fundecl) {
    assert(fundecl != NULL);

    if (fundecl->args) {
        free(fundecl->args);
    }

    block_free(fundecl->block);
}

void module_free(Module* module) {
    assert(module != NULL);

    if (module->fundecls) {
        for (int i = 0; i < module->fundecls_size; i++) {
            function_declaration_free(&module->fundecls[i]);
        }

        free(module->fundecls);
    }
}
