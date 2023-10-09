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
        struct node_expr exit_expr;
        struct {
            struct token ident;
            struct node_expr expr;
        } let_expr;
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

struct parser *p_init(struct token *tokens, const size_t token_count)
{
    struct parser *self = malloc(sizeof(struct parser));
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
    free(self);
}

// -----------------------------------------------
struct node_expr p_parse_expr(struct parser *self)
{
    struct node_expr expr;
    if (p_peek(self, 0) != NULL) {
        if (p_peek(self, 0)->type == TINT_LIT) {
            expr.type = EXPR_INT_LIT;
            expr.var.int_lit = *p_consume(self);
        }
        else if (p_peek(self, 0)->type == TIDENT) {
            expr.type = EXPR_IDENT;
            expr.var.ident = *p_consume(self);
        }
        else {
            expr.type = -1; // Invalid expression
        }
    }
    else {
        expr.type = -1; // Invalid expression
    }
    return expr;
}

struct node_stmt p_parse_stmt(struct parser *self)
{
    struct node_stmt stmt;
    if (p_peek(self, 0)->type == TEXIT && p_peek(self, 1) != NULL
        && p_peek(self, 1)->type == TPAREN_OPEN) {
        p_consume(self);
        p_consume(self);

        struct node_expr node_expr = p_parse_expr(self);
        if (node_expr.type != -1) {
            stmt.type = STMT_EXIT;
            stmt.var.exit_expr = node_expr;
        }
        else {
            fprintf(stderr, "[%d]: error: Invalid expression\n", __LINE__);
            goto fail;
        }

        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TPAREN_CLOSE) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "[ERROR] Expected `)`\n");
            goto fail;
        }
        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TSEMICOLON) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "[ERROR] Expected `;`\n");
            goto fail;
        }
    }
    else if (
        p_peek(self, 0) != NULL && p_peek(self, 0)->type == TLET
        && p_peek(self, 1) != NULL && p_peek(self, 1)->type == TIDENT
        && p_peek(self, 2) != NULL && p_peek(self, 2)->type == TEQUAL) {
        p_consume(self);

        struct node_stmt stmt;
        stmt.type = STMT_LET;
        stmt.var.let_expr.ident = *p_consume(self);

        p_consume(self);

        struct node_expr expr = p_parse_expr(self);
        if (expr.type != -1) {
            stmt.var.let_expr.expr = expr;
        }
        else {
            fprintf(stderr, "[%d]: error: Invalid expression\n", __LINE__);
            goto fail;
        }

        if (p_peek(self, 0) != NULL && p_peek(self, 0)->type == TSEMICOLON) {
            p_consume(self);
        }
        else {
            fprintf(stderr, "[ERROR] Expected `;`\n");
            goto fail;
        }
    }
    else {
        stmt.type = -1; // Invalid statement
    }

    return stmt;

fail:
    perror("[ERROR] Parser stmt\n");
    exit(EXIT_FAILURE);
}

struct node_prog *p_parse_prog(struct parser *self)
{
    struct node_prog *p = malloc(sizeof(struct node_prog));
    if (p == NULL) {
        perror("Failed to allocate memory for program");
        return NULL;
    }
    p->stmt_count = 0;
    p->stmts = NULL;
    p->stmts
        = malloc(10 * sizeof(struct node_stmt)); // Use malloc here, not calloc
    if (p->stmts == NULL) {
        perror("Failed to allocate memory for stmts");
        free(p);
        return NULL;
    }
    unsigned long n_loop_limit = 50;
    while (self->m_index < self->m_token_count) {
        if (n_loop_limit <= 0) {
            break;
        }
        struct node_stmt stmt = p_parse_stmt(self);
        if (stmt.type == -1) {
            int count_stmts = p->stmt_count;
            printf("count: %d\n", count_stmts);
            free(p->stmts);
            free(p);
            return NULL;
        }
        p->stmt_count++;
        p->stmts = realloc(p->stmts, p->stmt_count * sizeof(struct node_stmt));
        if (p->stmts == NULL) {
            fprintf(stderr, "Failed to reallocate memory\n");
            free(p->stmts);
            free(p);
            return NULL;
        }
        if (stmt.type >= 0) {
            p->stmts[p->stmt_count - 1] = stmt;
        }
        else {
            fprintf(stderr, "[%d]: error: Invalid statement\n", __LINE__);
            free(p->stmts);
            free(p);
            return NULL;
        }
        n_loop_limit--;
    }

    return p;
}

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
                    stmt.var.exit_expr.var.int_lit.value);
            }
            else if (stmt.type == STMT_LET) {
                printf(
                    "Let Statement: %s = %s\n",
                    stmt.var.let_expr.ident.value,
                    stmt.var.let_expr.expr.var.int_lit.value);
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

/* Print the parsed prog */
void p_node_prog_print(struct node_prog *self)
{
    printf("Parsed prog:\n");
    for (size_t i = 0; i < self->stmt_count; i++) {
        struct node_stmt stmt = self->stmts[i];
        if (stmt.type == STMT_EXIT) {
            printf(
                "Exit Statement with code: %s\n",
                stmt.var.exit_expr.var.int_lit.value);
        }
        else if (stmt.type == STMT_LET) {
            printf(
                "Let Statement: %s = %s\n",
                stmt.var.let_expr.ident.value,
                stmt.var.let_expr.expr.var.int_lit.value);
        }
    }
}

#endif /* DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00 */
