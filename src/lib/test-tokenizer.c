#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tokenizer.h"

// Enum to define error codes
static enum ErrorCodes {
    ERR_CMP_SUCCESS = 0, // Success
    ERR_CMP_TYPE_MISMATCH = -1, // Type mismatch error
    ERR_CMP_VALUE_MISMATCH = -2 // Value mismatch error
};

/**
 * 1. Check if the types are equal
 * 2. Check if both values are `NULL` or if their content is the same
 */
static int token_cmp_eq(
    struct token t1, struct token t2, char *err_msg, size_t err_msg_size)
{ // clang-format off
    if (t1.type != t2.type) {
        snprintf( err_msg, err_msg_size, "Expected type %d, got type %d", t2.type, t1.type);
        return ERR_CMP_TYPE_MISMATCH;
    }

    if ((t1.value != t2.value) && (t1.value == NULL || t2.value == NULL || strcmp(t1.value, t2.value) != 0)) {
        snprintf( err_msg, err_msg_size, "Expected value '%s', got value '%s'", t2.value, t1.value);
        return ERR_CMP_VALUE_MISMATCH;
    } // clang-format on

    return ERR_CMP_SUCCESS;
}

static void print_err(int line, const char *error_message)
{
    fprintf(stderr, "Error at line %d: %s\n", line, error_message);
}

static int test_tokens(void)
{
    char *src = "let x = 1;";
    struct token exp_tokens[] = {
        { .type = TOK_LET, .value = NULL },
        { .type = TOK_IDENT, .value = "x" },
        { .type = TOK_EQUAL, .value = NULL },
        { .type = TOK_INT_LIT, .value = "1" },
        { .type = TOK_SEMICOLON, .value = NULL },
    }; // expected hardcoded tokens for `let x = 1;`
    int exp_token_count = sizeof(exp_tokens) / sizeof(exp_tokens[0]);

    int mut_token_count = 0;
    struct tokenizer *tokenizer = tokenizer_init(src);
    struct token *tokens = tokenizer_tokenize(tokenizer, &mut_token_count);

    if (exp_token_count != mut_token_count) {
        print_err(__LINE__, "Token count mismatch");
        goto fail;
    }

    int i;
    char err_msg[256];
    for (i = 0; i < exp_token_count; i++) {
        int res
            = token_cmp_eq(tokens[i], exp_tokens[i], err_msg, sizeof(err_msg));
        if (res != ERR_CMP_SUCCESS) {
            print_err(__LINE__, err_msg);
            goto fail;
        }
    }

    return 0; // Successful

fail:
    tokenizer_free(tokenizer);
    token_free_tokens(tokens, mut_token_count);

    return -1; // Not Successful
}

int main(int argc, char *argv[])
{
    assert(0 == test_tokens());
    printf("Test passed: test_tokens\n");

    return 0;
}
