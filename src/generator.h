#ifndef A4541063_3A62_433A_A60C_275D5EB0263C
#define A4541063_3A62_433A_A60C_275D5EB0263C

#include "parser.h"
#include <stddef.h>

struct var {
    size_t stack_loc;
    // ... include more types for static typing
};

struct unordered_map {
    char *key;
    struct var var;
};

/** generator */
struct generator {
    struct var var;

    const struct node_prog m_prog; // std::stringstream m_output;

    char **m_output;

    /*
     * Our own stack pointer at compile time to move around the entity
     * offset of the penultimate item. copy that and add it to top of stack?
     * Limited numbers of registers wants us to utilize the Stack
     */
    size_t m_stack_size;

    /* Track variable's positions in stack */
    struct unordered_map m_vars;
};

/* impl public struct generator */

void generator_gen_term() {};
void generator_gen_bin_expr() {};
void generator_gen_expr() {};
void generator_gen_stmt() {};
void generator_gen_prog() {};

/* impl private struct generator */

static void generator_pop(struct generator *self, const char *reg)
{
    // self->m_output << "    push " << reg << "\n";
    self->m_stack_size++;
};

static void generator_push(struct generator *self, const char *reg)
{
    // self->m_output << "    pop " << reg << "\n";
    self->m_stack_size--;
};

#endif /* A4541063_3A62_433A_A60C_275D5EB0263C */
