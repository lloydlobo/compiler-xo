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
// struct generator {
//     struct var var;
//     struct node_prog m_prog;
//     char **m_output;
//     /* Stack Pointer at compile time */
//     size_t m_stack_size;
//     /* Track variable's positions in stack */
//     struct unordered_map m_vars;
// };
struct generator {
    struct var var;
    struct node_prog m_prog;
    char m_output[4096];
    /* Stack Pointer at compile time */
    size_t m_stack_size;
    /* Track variable's positions in stack */
    struct unordered_map m_vars;
};

// PRIVATE---------------------------------------------------------------------

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

// PUBLIC----------------------------------------------------------------------

struct generator g_init(struct node_prog *prog)
{
    struct generator self;
    memset(&self, 0, sizeof(struct generator));
    self.m_prog
        = *prog; // TODO: dup memory here to make m_prog static with const

    return self;
}

void g_gen_expr() {};
void g_gen_stmt(struct generator *self, const struct node_stmt stmt) {};

char *g_gen_prog(struct generator *self)
{
    const char *asm_header = "global _start\n_start:\n";
    sprintf((self->m_output + strlen(self->m_output)), "%s", asm_header);

    for (size_t i = 0, n = self->m_prog.stmt_count; i < n; i++)
        g_gen_stmt(self, self->m_prog.stmts[i]);

    /* If no explicit `exit()`, exit with 0, else this is unreachable in ASM. */
    strcat(self->m_output, "    mov rax, 60\n"); /* 60: exit symbol */
    strcat(self->m_output, "    mov rdi, 0\n"); /* 0: exit code */
    strcat(self->m_output, "    syscall\n"); /* call the kernel to execute */
    return self->m_output;
};

// ----------------------------------------------------------------------------

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
