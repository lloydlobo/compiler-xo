#ifndef A4541063_3A62_433A_A60C_275D5EB0263C
#define A4541063_3A62_433A_A60C_275D5EB0263C

#include "hashtable.h"
#include "lumlib.h"
#include "parser.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_HASHTABLE_SIZE 100 // count assigned to `size_t m_size_m_vars`

#define MAX_ASM_LINE_SIZE 80 // 24 => e.g. "    mov rax, 60\n"
#define BIT_MULTIPLIER 8
#define OFF_BY_ONE 1

struct g_var {
    size_t stack_loc;
    // ... include more types for static typing
};

/**
 * generator
 *
 */
struct generator {
    /* node prog from `struct parser` */
    struct node_prog m_prog;
    /* stringstream to emit assembly code */
    char m_output[4096];
    /*
     * Stack Pointer at compile time: * move around the entity offset of the
     * penultimate item. Due to Limited numbers of registers wants us to
     * utilize the Stack. Copy something and add it to top of stack.
     */
    size_t m_stack_size;
    /* Current size/count of the hash table */
    size_t m_vars_size;
    /*
     * Track variable's positions in stack
     * signature: `hashtable::<char *key, struct g_var **value>`
     */
    struct hashtable m_vars;
};

static void g_push(struct generator *self, const char *reg);
static void g_pop(struct generator *self, const char *reg);

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

// PERF: use tokens count in main.c to set `self.m_size_m_vars`
struct generator g_init(const struct node_prog *prog
                        /*, const size_t token_count */)
{
    struct generator self;
    memset(&self, 0, sizeof(struct generator));

    self.m_prog = *prog;
    self.m_stack_size = 0;
    self.m_output[0] = '\0';
    self.m_vars_size = INITIAL_HASHTABLE_SIZE;
    self.m_vars = *hashtable_init(self.m_vars_size);

    return self;
}

void g_gen_expr(struct generator *self, const struct node_expr *expr)
{ // printf("in gen_expr: %s\n", dbg_ident_val);
    char *const dbg_ident_val = expr->var.ident.value;
    if (expr->type == EXPR_INT_LIT) { // printf("in expr int_lit\n");
        strcat(self->m_output, "    mov rax, ");
        strcat(self->m_output, expr->var.int_lit.value);
        strcat(self->m_output, "\n");
        g_push(self, "rax");
    }
    else if (expr->type == EXPR_IDENT) {
        char *const ident_val = expr->var.ident.value;
        err_t err_found = hashtable_contains(&self->m_vars, ident_val);
        if (err_found != ErrOk) {
            assert(err_found != ErrInvalid && "Unreachable if not invalid");
            fprintf(stderr, "error: Undeclared identifier: %s\n", ident_val);
            goto fail;
        }
        struct g_var *var;
        err_t err_get = hashtable_get(&self->m_vars, ident_val, (void **)&var);
        if (err_get != ErrOk) {
            fprintf(stderr, "error: %s: %s\n", err_str(err_get), ident_val);
            goto fail;
        }
        char offset[MAX_ASM_LINE_SIZE];
        offset[0] = '\0';
        size_t offset_idx = (self->m_stack_size - var->stack_loc - OFF_BY_ONE)
            * BIT_MULTIPLIER; // clang-format off
        strcat(offset,           "QWORD [rsp + ");
        sprintf((offset + strlen(offset)), "%zu", offset_idx);
        strcat(offset,                         "]\n");
        g_push(self, offset); // clang-format on
    }

fail:
    fprintf(stderr, "%d: error: Failed to generate expression\n", __LINE__);
    return;
};

void g_gen_stmt(struct generator *self, const struct node_stmt *stmt)
{
    if (stmt->type == STMT_EXIT) {
        g_gen_expr(self, &stmt->var.expr_exit);
        strcat(self->m_output, "    mov rax, 60\n");
        g_pop(self, "rdi");
        strcat(self->m_output, "    syscall\n");
    }
    else if (stmt->type == STMT_LET) {
        const char *ident_val = stmt->var.expr_let.ident.value;
        err_t err = hashtable_contains(&self->m_vars, ident_val);
        if (err == ErrOk) {
            fprintf(stderr, "error: Identifier already used: %s\n", ident_val);
            goto fail;
        }
        assert(err == ErrNotFound && "stmt ident_val key maybe invalid");

        char *const key = stmt->var.expr_let.ident.value;
        struct g_var val = (struct g_var) { .stack_loc = self->m_stack_size };

        err = hashtable_insert(&self->m_vars, key, &val);
        if (err != ErrOk) {
            fprintf(stderr, "error: %s: in hashtable_insert\n", err_str(err));
            goto fail;
        }
        printf("%s\n", err_str(err));

        g_gen_expr(self, &stmt->var.expr_let.expr);
    }
    else if (stmt->type == STMT_INVALID) {
        fprintf(stderr, "error: Invalid statement");
        goto fail;
    }

fail:
    fprintf(stderr, "%d: error: Failed to generate statement\n", __LINE__);
}

char *g_gen_prog(struct generator *self)
{
    strcat(self->m_output, "global _start\n_start:\n");

    for (size_t i = 0, n = self->m_prog.stmt_count; i < n; i++) {
        g_gen_stmt(self, &self->m_prog.stmts[i]);
    }

    strcat(self->m_output, "    mov rax, 60\n"); /* 60: exit symbol */
    strcat(self->m_output, "    mov rdi, 0\n"); /* 0: exit code */
    strcat(self->m_output, "    syscall\n"); /* call the kernel to execute */

    return self->m_output;
};

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static void g_push(struct generator *self, const char *reg)
{ // printf("push: before: stack_size: %zu\n", self->m_stack_size);
    strcat(self->m_output, "    push ");
    strcat(self->m_output, reg);
    strcat(self->m_output, "\n");
    self->m_stack_size++; // printf("push: after: stack_size: %zu\n",
                          // self->m_stack_size);
};

static void g_pop(struct generator *self, const char *reg)
{ // printf("pop: before: stack_size: %zu\n", self->m_stack_size);
    strcat(self->m_output, "    pop ");
    strcat(self->m_output, reg);
    strcat(self->m_output, "\n");
    self->m_stack_size--; // printf("pop: after: stack_size: %zu\n",
                          // self->m_stack_size);
};

#endif /* A4541063_3A62_433A_A60C_275D5EB0263C */
