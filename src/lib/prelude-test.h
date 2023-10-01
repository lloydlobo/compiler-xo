#ifndef E673A0E2_6989_4CE7_A15A_17AFB5E7845F
#define E673A0E2_6989_4CE7_A15A_17AFB5E7845F

#include <stdio.h>

#include "../tokenizer.h"

/* Enum defines error codes */
enum ErrorCodes {
    ERR_CMP_SUCCESS = 0, // Success
    ERR_CMP_TYPE_MISMATCH = -1, // Type mismatch error
    ERR_CMP_VALUE_MISMATCH = -2 // Value mismatch error
};

void print_err(int line, const char *error_message)
{
    fprintf(stderr, "Error at line %d: %s\n", line, error_message);
}

/**
 * 1. Check if the types are equal
 * 2. Check if both values are `NULL` or if their content is the same
 */
int token_cmp_eq(
    struct token t1, struct token t2, char *err_msg, size_t err_msg_size)
{
    if (t1.type != t2.type) {
        snprintf(
            err_msg,
            err_msg_size,
            "Expected type %d, got type %d",
            t2.type,
            t1.type);
        return ERR_CMP_TYPE_MISMATCH;
    }

    if ((t1.value != t2.value)
        && (t1.value == NULL || t2.value == NULL
            || strcmp(t1.value, t2.value) != 0)) {
        snprintf(
            err_msg,
            err_msg_size,
            "Expected value '%s', got value '%s'",
            t2.value,
            t1.value);
        return ERR_CMP_VALUE_MISMATCH;
    }

    return ERR_CMP_SUCCESS;
}

#endif /* E673A0E2_6989_4CE7_A15A_17AFB5E7845F */
