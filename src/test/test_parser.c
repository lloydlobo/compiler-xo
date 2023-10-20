#include "include_test.h"

#include "../parser.h"
#include "../tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

// Function to print a node_stmt
void print_node_stmt(const struct node_stmt *stmt, int depth)
{
    for (int i = 0; i < depth; i++) {
        printf("  "); // Print indentation based on depth
    }

    // Print the statement type
    printf("Statement Type: %s\n", token_type_to_str(stmt->type));

    // Depending on the statement type, print additional information
    if (stmt->type == TOK_EXIT) {
        // Print the expression type for EXIT statements
        printf(
            "  Expression Type: %s\n",
            token_type_to_str(stmt->var.exit->expr->type));
    }
    else if (stmt->type == TOK_LET) {
        // Print the expression type for LET statements
        printf(
            "  Expression Type: %s\n",
            token_type_to_str(stmt->var.let->expr->type));
    }
}

// Function to recursively print the prog structure
void print_prog_recursive(const struct node_prog *prog, int depth)
{
    for (size_t i = 0; i < prog->stmt_count; i++) {
        // Print each statement
        print_node_stmt(&prog->stmts[i], depth);
    }
}

// ------------------------------------------------------------------------------

int test_parser_parse_prog(void)
{
    int mut_token_count = 0;

    const struct lib_src_tokens tmp_test_data[] = {
        { "exit(0);",
          (struct token[]) { { .type = TOK_EXIT },
                             { .type = TOK_PAREN_OPEN },
                             { .type = TOK_INT_LIT, .value = "0" },
                             { .type = TOK_PAREN_CLOSE },
                             { .type = TOK_SEMICOLON } } },
    };

    struct tokenizer *tokenizer = tokenizer_init(tmp_test_data[0].m_src);
    struct token *tokens = tokenizer_tokenize(tokenizer, &mut_token_count);
    tokenizer_free(tokenizer);

    struct parser *parser = parser_init(tokens, mut_token_count);
    {
        struct node_prog *prog = parser_parse_prog(parser);
        // Print the entire prog structure as a tree-linked list
        printf("Program Structure:\n");
        print_prog_recursive(prog, 0);
        if (prog == NULL) {
            printf("Invalid program at --%d--\n", __LINE__);
            goto fail;
        }
        printf("Statement count: %zu\n", prog->stmt_count);

        // Print the entire prog structure as a tree-linked list
        printf("Program Structure:\n");
        print_prog_recursive(prog, 0);
    }

    parser_free(parser);
    token_free_tokens(tokens, mut_token_count);

    return 0; // Successful

fail:

    parser_free(parser);
    token_free_tokens(tokens, mut_token_count);

    return -1; // Not Successful
}

// ------------------------------------------------------------------------------

int test_new_parser_prog_v1(void)
{
    unsigned long len = 1000;
    int size = sizeof(struct node_prog);
    printf("size: %d\n", size);
    struct node_prog *prog = (struct node_prog *)malloc(len * size);
    // ERR_DBG_LN("");
    printf("count: %zu\n", prog->stmt_count);

    struct node_stmt stmt
        = { .type = TOK_EXIT,
            .var.exit = &((struct node_stmt_exit) {
                .expr = &((struct node_expr) {
                    .type = TOK_EXIT,
                    .var.ident = &((struct node_expr_ident) {
                        .ident = ((struct token) {
                            .type = TOK_EXIT, .value = NULL }) }) }) }) };

    unsigned long initial_stmt_count = 40;
    prog->stmts = calloc(initial_stmt_count, (sizeof(struct node_stmt)));
    prog->stmts[prog->stmt_count] = stmt;
    prog->stmt_count++;
    if (initial_stmt_count > 40) {
        prog->stmts = realloc(
            prog->stmts, (initial_stmt_count + 1) * (sizeof(struct node_stmt)));
    }
    printf("count: %zu\n", prog->stmt_count);

    // .var.exit = NULL };
    // ERR_DBG_LN("");
    printf("%zu: %p\n", prog->stmt_count, &prog->stmts);

    return 0; // Successful
}

int test_new_parser_prog_v2(void)
{
    unsigned long len = 1000;

    struct node_prog *prog = malloc(sizeof(struct node_prog));
    if (prog == NULL) {
        perror("Failed to allocate memory for prog");
        return -1; // Error: Memory allocation failed
    }

    unsigned long initial_stmt_count = 40;
    prog->stmt_count = 0;
    prog->stmts = calloc(initial_stmt_count, sizeof(struct node_stmt));
    if (prog->stmts == NULL) {
        perror("Failed to allocate memory for stmts");
        free(prog); // Clean up the allocated memory for prog
        return -1; // Error: Memory allocation failed
    }

    struct node_stmt stmt
        = { .type = TOK_EXIT,
            .var.exit = &(struct node_stmt_exit) {
                .expr = &(struct node_expr) {
                    .type = TOK_EXIT,
                    .var.ident = &(struct node_expr_ident) {
                        .ident = (struct token) {
                            .type = TOK_EXIT, .value = NULL } } } } };

    if (prog->stmt_count < initial_stmt_count) {
        prog->stmts[prog->stmt_count++] = stmt;
    }
    else {
        // Resize stmts array if needed
        unsigned long new_size = initial_stmt_count * 2;
        struct node_stmt *new_stmts
            = realloc(prog->stmts, new_size * sizeof(struct node_stmt));
        if (new_stmts == NULL) {
            perror("Failed to resize stmts array");
            free(prog->stmts); // Clean up the allocated memory for stmts
            free(prog); // Clean up the allocated memory for prog
            return -1; // Error: Memory reallocation failed
        }
        prog->stmts = new_stmts;
        prog->stmts[prog->stmt_count++] = stmt;
    }

    printf("Statement count: %zu\n", prog->stmt_count);

    free(prog->stmts);
    free(prog);

    return 0; // Successful
}

int test_new_parser_prog_v3(void)
{
    unsigned long len = 1000;

    struct node_prog *prog = malloc(sizeof(struct node_prog));
    if (prog == NULL) {
        perror("Failed to allocate memory for prog");
        return -1; // Error: Memory allocation failed
    }

    unsigned long initial_stmt_count = 40;
    prog->stmt_count = 0;
    prog->stmts = calloc(initial_stmt_count, sizeof(struct node_stmt));
    if (prog->stmts == NULL) {
        perror("Failed to allocate memory for stmts");
        free(prog); // Clean up the allocated memory for prog
        return -1; // Error: Memory allocation failed
    }

    struct node_stmt stmt
        = { .type = TOK_EXIT,
            .var.exit = &(struct node_stmt_exit) {
                .expr = &(struct node_expr) {
                    .type = TOK_EXIT,
                    .var.ident = &(struct node_expr_ident) {
                        .ident = (struct token) {
                            .type = TOK_EXIT, .value = 0 } } } } };

    if (prog->stmt_count < initial_stmt_count) {
        prog->stmts[prog->stmt_count++] = stmt;
    }
    else {
        // Resize stmts array if needed
        unsigned long new_size = initial_stmt_count * 2;
        struct node_stmt *new_stmts
            = realloc(prog->stmts, new_size * sizeof(struct node_stmt));
        if (new_stmts == NULL) {
            perror("Failed to resize stmts array");
            free(prog->stmts); // Clean up the allocated memory for stmts
            free(prog); // Clean up the allocated memory for prog
            return -1; // Error: Memory reallocation failed
        }
        prog->stmts = new_stmts;
        prog->stmts[prog->stmt_count++] = stmt;
    }

    printf("Statement count: %zu\n", prog->stmt_count);

    // Print the entire prog structure as a tree-linked list
    printf("Program Structure:\n");
    print_prog_recursive(prog, 0);

    free(prog->stmts);
    free(prog);

    return 0; // Successful
}

// ------------------------------------------------------------------------------

int main(void)
{
    // assert(0 == test_new_parser_prog_v1());
    // printf("Test passed: test_new_parser_prog_v1\n");

    // assert(0 == test_new_parser_prog_v2());
    // printf("Test passed: test_new_parser_prog_v2\n");

    // assert(0 == test_new_parser_prog_v3());
    // printf("Test passed: test_new_parser_prog_v3\n");

    assert(0 == test_parser_parse_prog());
    printf("Test passed: test_parser\n");

    return 0;
}
