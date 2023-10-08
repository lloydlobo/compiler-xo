#include "include_test.h"

#include "../tokenizer.h"

static int test_tokens_stmts(void)
{
    struct tokenizer *tokenizer;
    struct token *tokens;
    int mut_token_count;

    size_t i;
    size_t n
        = sizeof(lib_src_tokens_data) / sizeof(const struct lib_src_tokens);

    for (i = 0; i < n; i++) {
        mut_token_count = 0;
        tokenizer = tokenizer_init(lib_src_tokens_data[i].m_src);
        tokens = tokenizer_tokenize(tokenizer, &mut_token_count);

        printf(
            "--%d-- i: %zu stmt: %s\n",
            __LINE__,
            i,
            lib_src_tokens_data[i].m_src); // DEBUG

        int j;
        char err_msg[256];
        while (j < mut_token_count) {
            int res = lib_token_cmp_eq(
                tokens[j],
                lib_src_tokens_data[i].m_tokens[j],
                err_msg,
                sizeof(err_msg));
            if (res != ERR_CMP_SUCCESS) {
                printf("Token %d mismatch: %s\n", j, err_msg);
                goto fail;
            }
            j++;
        }

        if (mut_token_count != j) {
            printf(
                "Token count mismatch: expected %d, got %d\n",
                j,
                mut_token_count);
            goto fail;
        }

        tokenizer_free(tokenizer);
        token_free_tokens(tokens, mut_token_count);
        printf("Test Case %zu: PASSED\n", i);
    }

    return 0; // Successful

fail:

    tokenizer_free(tokenizer);
    token_free_tokens(tokens, mut_token_count);

    return -1; // Not Successful
}

int main(void)
{
    assert(0 == test_tokens_stmts());
    printf("Test passed: test_tokens_stmts\n");

    return 0;
}
