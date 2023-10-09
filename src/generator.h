#ifndef A4541063_3A62_433A_A60C_275D5EB0263C
#define A4541063_3A62_433A_A60C_275D5EB0263C

#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct var {
    size_t stack_loc;
    // ... include more types for static typing
};

struct unordered_map {
    char *key;
    struct var var;
};

/**
 * generator
 *
 * stack size: move around the entity offset of the penultimate item.
 *   Due to Limited numbers of registers wants us to utilize the Stack
 *   Copy something and add it to top of stack..
 */
struct generator {
    struct node_prog m_prog; /* prog from `struct parser` */
    char m_output[4096]; /* stringstream to emit assembly code */
    size_t m_stack_size; /* Stack Pointer at compile time */

    // struct var var;
    // struct unordered_map m_vars; /* Track variable's positions in stack */
};

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static void g_push(struct generator *self, const char *reg)
{
    strcat(self->m_output, "    push ");
    strcat(self->m_output, reg);
    strcat(self->m_output, "\n");
    self->m_stack_size--;
};

static void g_pop(struct generator *self, const char *reg)
{
    strcat(self->m_output, "    pop ");
    strcat(self->m_output, reg);
    strcat(self->m_output, "\n");
    self->m_stack_size++;
};

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct generator g_init(struct node_prog *prog)
{
    struct generator self;
    memset(&self, 0, sizeof(struct generator));

    self.m_prog = *prog;
    self.m_stack_size = 0;
    self.m_output[0] = '\0';

    return self;
}

void g_gen_expr(struct generator *self, const struct node_expr *expr)
{
    switch (expr->type) {
    case EXPR_INT_LIT:
        strcat(self->m_output, "    mov rax, ");
        strcat(self->m_output, expr->var.int_lit.value);
        strcat(self->m_output, "\n");
        g_push(self, "rax");
        break;
    case EXPR_IDENT:
        strcat(self->m_output, "    mov rax, ");
        strcat(self->m_output, expr->var.ident.value);
        strcat(self->m_output, "\n");
        g_push(self, "rax");
        break;
    case EXPR_INVALID:
        break;
    }
};

void g_gen_stmt(struct generator *self, const struct node_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_EXIT:
        g_gen_expr(self, &stmt->var.exit_expr);
        strcat(self->m_output, "    mov rax, 60\n");
        strcat(self->m_output, "    pop rdi\n"); // expr can be 2 things
        strcat(self->m_output, "    syscall\n");
        break;
    case STMT_LET:
        // TODO:
        break;
    case STMT_INVALID:
        break;
    }
};

char *g_gen_prog(struct generator *self)
{
    const char *asm_header = "global _start\n_start:\n";
    strcat(self->m_output, asm_header);

    for (size_t i = 0, n = self->m_prog.stmt_count; i < n; i++) {
        g_gen_stmt(self, &self->m_prog.stmts[i]);
    }

    /*
     * If no explicit `exit()`, exit with 0,
     * else this is unreachable in ASM.
     */
    strcat(self->m_output, "    mov rax, 60\n"); /* 60: exit symbol */
    strcat(self->m_output, "    mov rdi, 0\n"); /* 0: exit code */
    strcat(self->m_output, "    syscall\n"); /* call the kernel to execute */

    return self->m_output;
};

// —————————————————————————————————————————————————————————————————————————————————————
// EOF

/*
 * A buffer size of 24 characters accommodates both 32-bit and 64-bit
 * hexadecimal addresses comfortably. For example, "0xdeadbeef" (32-bit) and
 * "0xdeadbeefdeadbeef" (64-bit) can fit.
 * - `push 0xdeadbeef` is 15 characters long. `pop 0xdeadbeefdeadbeef` is 20
 * characters long. */
// const size_t MAX_BUF_SIZE = 24;
// static void g_push(struct generator *self, const char *reg) {
// char buf[MAX_BUF_SIZE];
// const char *asm_push = "    push ";
// int chars_written = snprintf(buf, sizeof(buf), "%s%s\n", asm_push, reg);
// if (chars_written < 0 || chars_written >= sizeof(buf)) {
//     fprintf(stderr, "Error formatting output for register %s\n", reg);
//     exit(EXIT_FAILURE); }
// self->m_output[self->m_stack_size] = strdup(buf);
// if (self->m_output[self->m_stack_size] == NULL) {
//     perror("Error allocating memory for output buffer");
//     exit(EXIT_FAILURE); }
// self->m_stack_size--;
//};

/*
 * [Source](https://github.com/Wilfred/babyc/blob/0206e3a1c435f9cdf37b5ffbc3a05c522db5cda7/assembly.c#L256C32-L256C32)
 */
/*
void write_assembly(Syntax *syntax)
{
    FILE *out = fopen("out.s", "wb");

    write_header(out);

    Context *ctx = new_context();

    write_syntax(out, syntax, ctx);
    write_footer(out);

    context_free(ctx);
    fclose(out);
}
*/

#endif /* A4541063_3A62_433A_A60C_275D5EB0263C */
