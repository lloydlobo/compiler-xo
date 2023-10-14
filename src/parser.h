#ifndef DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00
#define DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00

#include "tokenizer.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum NodeExprType {
    EXPR_INT_LIT,
    EXPR_IDENT,
    EXPR_INVALID = -1,
};

struct node_expr {
    enum NodeExprType type;
    union {
        struct token int_lit;
        struct token ident;
    } var;
};

enum NodeStmtType {
    STMT_EXIT,
    STMT_LET,
    STMT_INVALID = -1,
};

struct node_stmt {
    enum NodeStmtType type;
    union {
        struct node_expr expr_exit;
        struct {
            struct token ident;
            struct node_expr expr;
        } expr_let;
    } var;
};

struct node_prog {
    struct node_stmt *stmts;
    size_t stmt_count;
};

struct parser {
    struct token *m_tokens;
    size_t m_token_count;
    size_t m_index;
};

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static struct token *p_peek(const struct parser *self, int offset)
{
    if (self->m_index + offset >= self->m_token_count)
        return NULL;
    return &self->m_tokens[self->m_index + offset];
}

static struct token *p_consume(struct parser *self)
{
    return &self->m_tokens[(self->m_index)++];
}

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct parser *p_init(struct token *tokens, const size_t token_count)
{
    struct parser *self = (struct parser *)malloc(sizeof(struct parser));
    if (self == NULL) {
        perror("Failed to allocate memory for Parser\n");
        return NULL;
    }
    if (tokens == NULL || token_count == 0) {
        perror("Empty tokens array passed while creating parser\n");
        self->m_tokens = NULL;
    }
    else {
        self->m_tokens = tokens;
    }
    self->m_index = 0;
    self->m_token_count = token_count;
    return self;
}

void p_free(struct parser *self)
{
    if (self != NULL)
        free(self);
}

struct node_expr p_parse_expr(struct parser *self)
{
    if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TINT_LIT) {
        return (struct node_expr) { .type = EXPR_INT_LIT,
                                    .var.int_lit = *p_consume(self) };
    }
    else if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TIDENT) {
        return (struct node_expr) { .type = EXPR_IDENT,
                                    .var.ident = *p_consume(self) };
    }
    else {
        return (struct node_expr) { .type = EXPR_INVALID };
    }
}

struct node_stmt p_parse_stmt(struct parser *self)
{
    struct node_stmt stmt;

    if (p_peek(self, 0)->type == TEXIT && p_peek(self, 1) != NULL
        && p_peek(self, 1)->type == TPAREN_OPEN) {
        assert(p_consume(self)->type == TEXIT);
        assert(p_consume(self)->type == TPAREN_OPEN);
        struct node_expr node_expr = p_parse_expr(self);
        if (node_expr.type != EXPR_INVALID) {
            stmt.type = STMT_EXIT;
            stmt.var.expr_exit = node_expr;
        }
        else {
            fprintf(stderr, "error: Invalid expression\n");
            goto fail;
        }
        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TPAREN_CLOSE) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "error: Expected `)`\n");
            goto fail;
        }
        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TSEMICOLON) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "error: Expected `;`\n");
            goto fail;
        }
    }
    else if (
        p_peek(self, 0) != NULL && p_peek(self, 0)->type == TLET
        && p_peek(self, 1) != NULL && p_peek(self, 1)->type == TIDENT
        && p_peek(self, 2) != NULL && p_peek(self, 2)->type == TEQUAL) {
        assert(p_consume(self)->type == TLET);
        stmt.type = STMT_LET; // (let) `let x = 1;`
        stmt.var.expr_let.ident = *p_consume(self);
        assert(p_consume(self)->type == TEQUAL);

        struct node_expr nexpr = p_parse_expr(self);
        if (nexpr.type != EXPR_INVALID) {
            stmt.var.expr_let.expr = nexpr;
        }
        else {
            fprintf(stderr, "error: Invalid expression\n");
            goto fail;
        }
        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TSEMICOLON) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "error: Expected `;`\n");
            goto fail;
        }
    }
    else if (
        p_peek(self, 0) != NULL
        && (p_peek(self, 0)->type != TLET
            && (p_peek(self, 1) != NULL && p_peek(self, 1)->type == TCOLON)
            && (p_peek(self, 0) != NULL && p_peek(self, 2)->type == TEQUAL))) {
        stmt.type = STMT_LET; // (walrus) `x := 1;`
        stmt.var.expr_let.ident = *p_consume(self);

        assert(p_consume(self)->type == TCOLON);
        assert(p_consume(self)->type == TEQUAL);

        struct node_expr nexpr = p_parse_expr(self);
        if (nexpr.type != EXPR_INVALID) {
            stmt.var.expr_let.expr = nexpr;
        }
        else {
            fprintf(stderr, "error: Invalid expression\n");
            goto fail;
        }
        if (p_peek(self, 0) != NULL
            && p_peek(self, 0)->type == TSEMICOLON) { /* TODO \n without semi */
            p_consume(self);
        }
        else {
            fprintf(stderr, "error: Expected `;`\n");
            goto fail;
        }
    }
    else {
        perror("Syntax error\n");
        stmt.type = STMT_INVALID;
    }

    return stmt;

fail:
    perror("error: Failed to parse statements\n");
    exit(EXIT_FAILURE);
}

struct node_prog *p_parse_prog(struct parser *self)
{
    struct node_prog *p = (struct node_prog *)malloc(sizeof(struct node_prog));
    if (p == NULL) {
        fprintf(stderr, "error: Failed to allocate memory for program\n");
        return NULL;
    }
    p->stmt_count = 0;
    p->stmts = (struct node_stmt *)malloc(10 * sizeof(struct node_stmt));
    if (p->stmts == NULL) {
        fprintf(stderr, "error: Failed to allocate memory for stmts\n");
        goto err_cleanup;
    }

    while (p_peek(self, 0) != NULL) {
        struct node_stmt stmt = p_parse_stmt(self);
        if (stmt.type == STMT_INVALID) {
            fprintf(stderr, "error: Invalid statement\n");
            goto err_cleanup;
        }
        p->stmt_count++;
        p->stmts = (struct node_stmt *)realloc(
            p->stmts, p->stmt_count * sizeof(struct node_stmt));
        if (p->stmts == NULL) {
            fprintf(stderr, "error: Failed to reallocate memory\n");
            goto err_cleanup;
        }
        p->stmts[p->stmt_count - 1] = stmt;
    }
    return p;

err_cleanup:
    if (p->stmts != NULL)
        free(p->stmts);
    free(p);
    return NULL;
}

/* Print the parsed prog */
void p_node_prog_print(struct node_prog *self)
{
    printf("Parsed prog:\n");
    for (size_t i = 0; i < self->stmt_count; i++) {
        struct node_stmt stmt = self->stmts[i];
        if (stmt.type == STMT_EXIT) {
            printf(
                "Exit Statement with code: %s\n",
                stmt.var.expr_exit.var.int_lit.value);
        }
        else if (stmt.type == STMT_LET) {
            printf(
                "Let Statement: %s = %s\n",
                stmt.var.expr_let.ident.value,
                stmt.var.expr_let.expr.var.int_lit.value);
        }
    }
}

// —————————————————————————————————————————————————————————————————————————————————————
// Test

static int run_main()
{
    struct token tokens[]
        = { { TEXIT, NULL },
            { TPAREN_OPEN, NULL },
            { TINT_LIT, "42" },
            { TPAREN_CLOSE, NULL },
            { TSEMICOLON, NULL } };

    size_t token_count = sizeof(tokens) / sizeof(tokens[0]);

    struct parser *p = p_init(tokens, token_count);

    struct node_prog *program = p_parse_prog(p);

    if (program != NULL) {
        // Print the parsed program
        printf("Parsed Program:\n");
        for (size_t i = 0; i < program->stmt_count; i++) {
            struct node_stmt stmt = program->stmts[i];
            if (stmt.type == STMT_EXIT) {
                printf(
                    "Exit Statement with code: %s\n",
                    stmt.var.expr_exit.var.int_lit.value);
            }
            else if (stmt.type == STMT_LET) {
                printf(
                    "Let Statement: %s = %s\n",
                    stmt.var.expr_let.ident.value,
                    stmt.var.expr_let.expr.var.int_lit.value);
            }
        }

        // Free memory allocated for the program
        free(program->stmts);
        free(program);
    }

    // Free the parser
    p_free(p);

    return 0;
}

#endif /* DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00 */
