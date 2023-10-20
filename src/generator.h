#ifndef A4541063_3A62_433A_A60C_275D5EB0263C
#define A4541063_3A62_433A_A60C_275D5EB0263C

#include "lumlib.h"
#include "parser.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASM_STRINGSTREAM_OUTPUT_SIZE 4096
#define MAX_ASM_LINE_SIZE 80 // 24 => e.g. "    mov rax, 60\n"
#define BIT_MULTIPLIER 8
#define OFF_BY_ONE 1

// —————————————————————————————————————————————————————————————————————————————————————
// VAR CLASS

struct var {
        size_t stack_loc;
        char  *ident_name;
        // ... include more types for static typing
        // size_t l_start; size_t l_end; size_t c_start; size_t c_end;
};

struct var *var_init_array(size_t count, size_t capacity);
void        var_free_array(struct var *self, const int var_count);
struct var  var_arr_at(struct var *self, size_t size, size_t index);
void        var_print_array(struct var *self, size_t size);

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct var *var_init_array(size_t count, size_t capacity) {
        assert(count <= capacity);
        size_t size = sizeof(struct var);

        struct var *self_array = (struct var *)calloc(count, size);
        if (self_array == NULL) {
                fprintf(stderr, "error: %s in var_new\n", err_str(ErrNoMem));
                return NULL;
        }

        return self_array;
}
void var_free_array(struct var *self, const int var_count) {
        if (self == NULL)
                return;

        for (int i = 0; i < var_count; i++)
                if (self[i].ident_name != NULL)
                        free(self[i].ident_name);

        free(self);
}
struct var var_arr_at(struct var *self, size_t size, size_t index) {
        return self[index];
}
void var_print_array(struct var *self, size_t size) {
        for (int i = 0; i < size; i++) { // clang-format off
        printf( "--%d-- Var %d: stack_loc:[%zu] ident_name:'%s'\n",
            __LINE__, i, self[i].stack_loc, self[i].ident_name);
    } // clang-format on
};

// —————————————————————————————————————————————————————————————————————————————————————
// GENERATOR CLASS

/**
 * generator
 */
struct generator {
        struct node_prog m_prog; /* node prog from `struct parser` */
        char             m_output[ASM_STRINGSTREAM_OUTPUT_SIZE]; /* stringstream to emit
                                                                    assembly code */
        size_t      m_stack_size; /* Stack Pointer at compile time: */
        size_t      m_vars_size;
        size_t      m_vars_capacity;
        struct var *m_vars;

        // size_t m_vars_size; /* Current size/count of the hash table */
        // struct hashtable m_vars; /* Track variable's information in stack */
};

struct generator generator_init(const struct node_prog *prog);
void             generator_free(struct generator *self);
void             generator_gen_expr(struct generator *self, const struct node_expr *expr);
void             generator_gen_stmt(struct generator *self, const struct node_stmt *stmt);
char            *generator_gen_prog(struct generator *self);

static void  generator_push(struct generator *self, const char *reg);
static void  generator_pop(struct generator *self, const char *reg);
static bool  generator_vars_contains_ident(struct generator *self, const char *ident);
static err_t generator_vars_push_back_var(struct generator *self, const struct var var);

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct generator generator_init(const struct node_prog *prog) {
        // PERF: use tokens count in main.c to set `self.m_size_m_vars`?
        struct generator self;
        struct var      *vars;

        memset(&self, 0, sizeof(struct generator));

        self.m_output[0] = '\0';
        self.m_prog = *prog;
        self.m_stack_size = 0;
        self.m_vars_size = 0;
        self.m_vars_capacity = self.m_prog.stmt_count;

        vars = var_init_array(self.m_prog.stmt_count, self.m_vars_capacity);
        if (vars == NULL) {
                fprintf(stderr, "error: %s at var_init_array\n", err_str(ErrNoMem));
                exit(EXIT_FAILURE);
        }
        self.m_vars = vars; // memcpy this?

        return self;
}

void generator_free(struct generator *self) {
        if (self == NULL)
                return;

        var_print_array(self->m_vars, self->m_vars_size);

        if (self->m_vars != NULL) {
                var_free_array(self->m_vars, self->m_vars_size);
        }
        // Invalid free() / delete / delete[] / realloc()
        // free(self); // due to memset on stack with
}

void emit_function(struct generator *self) {
        assert(false && "unimplemented");
}

void generator_gen_expr(struct generator *self, const struct node_expr *expr) {
        if (expr->type == EXPR_INT_LIT) {
                strcat(self->m_output, "    mov rax, ");
                strcat(self->m_output, expr->var.int_lit.value);
                strcat(self->m_output, "\n");
                generator_push(self, "rax");
        } else if (expr->type == EXPR_IDENT) {
                char  *var_ident_name = NULL;
                size_t var_stack_loc;
                char  *ident_val;

                ident_val = expr->var.ident.value;
                if (!generator_vars_contains_ident(self, ident_val)) {
                        fprintf(stderr, "error: Undeclared identifier: %s\n", ident_val);
                        goto fail;
                }
                for (size_t i = 0; i < self->m_vars_size; i += 1) {
                        struct var cur = self->m_vars[i];
                        if (strcmp(cur.ident_name, ident_val) == 0) {
                                var_ident_name = cur.ident_name;
                                var_stack_loc = cur.stack_loc;
                                break;
                        }
                }
                if (var_ident_name == NULL) {
                        const char *err_msg = err_str(ErrNotFound);
                        fprintf(stderr, "error: %s: identifier %s\n", err_msg, ident_val);
                        goto fail;
                }

                char offset[MAX_ASM_LINE_SIZE];
                offset[0] = '\0';
                size_t offset_idx = (self->m_stack_size - var_stack_loc - OFF_BY_ONE) *
                                    BIT_MULTIPLIER; // clang-format off
        strcat(offset,           "QWORD [rsp + ");
        sprintf((offset + strlen(offset)), "%zu", offset_idx);
        strcat(offset,                         "]\n");
        generator_push(self, offset); // clang-format on
        } else if (expr->type == EXPR_INVALID) {
                fprintf(stderr, "error: Invalid expression");
                goto fail;
        }

fail:
        fprintf(stderr, "%d: error: Failed to generate expression\n", __LINE__);
        return;
};

void generator_gen_stmt(struct generator *self, const struct node_stmt *stmt) {
        if (stmt->type == STMT_EXIT) {
                generator_gen_expr(self, &stmt->var.expr_exit);

                strcat(self->m_output, "    mov rax, 60\n");
                generator_pop(self, "rdi");
                strcat(self->m_output, "    syscall\n");
        } else if (stmt->type == STMT_LET_IMUT) {
                struct var  var;
                const char *ident_val;

                ident_val = stmt->var.expr_let.ident.value;
                if (generator_vars_contains_ident(self, ident_val)) {
                        fprintf(stderr, "error: Identifier already used: %s\n", ident_val);
                        goto fail;
                }
                var = (struct var){
                    .stack_loc = self->m_stack_size,
                    .ident_name = strdup(stmt->var.expr_let.ident.value),
                };
                err_t err = generator_vars_push_back_var(self, var);
                if (err != ErrOk) {
                        fprintf(stderr, "error: %s: %s\n", err_str(err), ident_val);
                        goto fail;
                }

                generator_gen_expr(self, &stmt->var.expr_let.expr);
        } else if (stmt->type == STMT_LET_MUT) {
                struct var  var;
                const char *ident_val;

                ident_val = stmt->var.expr_let.ident.value;
                if (generator_vars_contains_ident(self, ident_val)) {
                        fprintf(stderr, "error: Identifier already used: %s\n", ident_val);
                        goto fail;
                }
                var = (struct var){
                    .stack_loc = self->m_stack_size,
                    .ident_name = strdup(stmt->var.expr_let.ident.value),
                };
                err_t err = generator_vars_push_back_var(self, var);
                if (err != ErrOk) {
                        fprintf(stderr, "error: %s: %s\n", err_str(err), ident_val);
                        goto fail;
                }

                generator_gen_expr(self, &stmt->var.expr_let.expr);
        } else if (stmt->type == STMT_INVALID) {
                fprintf(stderr, "error: Invalid statement");
                goto fail;
        }

fail:
        fprintf(stderr, "%d: error: Failed to generate statement\n", __LINE__);
}

char *generator_gen_prog(struct generator *self) {
        strcat(self->m_output, "global _start\n_start:\n");

        for (size_t i = 0, n = self->m_prog.stmt_count; i < n; i++) {
                generator_gen_stmt(self, &self->m_prog.stmts[i]);
                // var_print_array(self->m_vars, self->m_vars_size); // dbg!
        }

        strcat(self->m_output, "\n");
        strcat(self->m_output, "    mov rax, 60\n"); /* 60: exit symbol */
        strcat(self->m_output, "    mov rdi, 0\n");  /* 0: exit code */
        strcat(self->m_output, "    syscall\n");     /* call the kernel to execute */

        return self->m_output;
};

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static void generator_push(struct generator *self, const char *reg) {
        strcat(self->m_output, "    push ");
        strcat(self->m_output, reg);
        strcat(self->m_output, "\n");
        self->m_stack_size += 1;
};

static void generator_pop(struct generator *self, const char *reg) {
        strcat(self->m_output, "    pop ");
        strcat(self->m_output, reg);
        strcat(self->m_output, "\n");
        self->m_stack_size -= 1;
};

static err_t generator_vars_push_back_var(struct generator *self, const struct var var) {
        if (self->m_vars_size > self->m_vars_capacity)
                return ErrEnd;

        if (self->m_vars == NULL || var.ident_name == NULL || var.stack_loc < 0)
                return ErrInvalid;

        self->m_vars[self->m_vars_size] = (struct var){
            .stack_loc = var.stack_loc,
            .ident_name = var.ident_name,
        };
        self->m_vars_size += 1;

        return ErrOk;
}

static bool generator_vars_contains_ident(struct generator *self, const char *ident) {
        if (self->m_vars_size == 0)
                return false;

        for (size_t i = 0; i < self->m_vars_size; i++) {
                if (strcmp(self->m_vars[i].ident_name, ident) == 0)
                        return true;
        }

        return false;
}

#endif /* A4541063_3A62_433A_A60C_275D5EB0263C */
