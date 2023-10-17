#ifndef CF17C85F_D892_43F8_B47B_0BA265F170E6
#define CF17C85F_D892_43F8_B47B_0BA265F170E6

#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_DBG(msg) fprintf(stderr, "--%d-- %s ", __LINE__, msg);
#define ERR_DBG_LN(msg) fprintf(stderr, "--%d-- %s\n", __LINE__, msg);

static int buf_clear(char array[], size_t *array_length)
{
    if (array == NULL)
        return -EBUSY; // see linux style guide
    for (int i = 0; i < *array_length; i++) {
        array[i] = '\0';
    }
    *array_length = 0;

    return 0; // success
}

/*
 * Define macros to convert enum values to strings
 */
#define ENUM_TO_STR(e) #e
#define CASE_ENUM_TO_STR(e) \
    case e: \
        return #e

/* TokenType */
enum TokenType {
    TEXIT,
    TINT_LIT,
    TSEMICOLON,
    TCOLON,
    TPAREN_OPEN,
    TPAREN_CLOSE,
    TCURLY_OPEN,
    TCURLY_CLOSE,
    TIDENT,
    TLET,
    TEQUAL,
    TCOMMENT,
};

const char *tokentype_to_str(enum TokenType self)
{
    switch (self) {
        CASE_ENUM_TO_STR(TEXIT);
        CASE_ENUM_TO_STR(TINT_LIT);
        CASE_ENUM_TO_STR(TSEMICOLON);
        CASE_ENUM_TO_STR(TCOLON);
        CASE_ENUM_TO_STR(TPAREN_OPEN);
        CASE_ENUM_TO_STR(TPAREN_CLOSE);
        CASE_ENUM_TO_STR(TCURLY_OPEN);
        CASE_ENUM_TO_STR(TCURLY_CLOSE);
        CASE_ENUM_TO_STR(TIDENT);
        CASE_ENUM_TO_STR(TLET);
        CASE_ENUM_TO_STR(TEQUAL);
        CASE_ENUM_TO_STR(TCOMMENT);
    }
    return "Unknown"; // Handle the case where self is not recognized.
}

/** token */
struct token {
    enum TokenType type;
    char *value;
};

/* impl struct token */

void token_free_tokens(struct token *self, int token_count)
{
    if (self == NULL)
        return;

    for (int i = 0; i < token_count; i++) { // clang-format off
        printf( "--%d-- Token %d: %u %s %s\n", __LINE__, i, self[i].type,
            tokentype_to_str(self[i].type), self[i].value); // clang-format on
        if (self[i].value != NULL)
            free(self[i].value);
    }
    free(self);
}

/** tokenizer */
struct tokenizer {
    char *m_src;
    size_t m_index;

    size_t code_lines_processed;
    size_t total_code_lines_processed;
};

/* impl struct tokenizer */

struct tokenizer *t_init(const char *src)
{
    size_t size = sizeof(struct tokenizer);
    struct tokenizer *self = (struct tokenizer *)malloc(size);
    if (self == NULL) {
        perror("Failed to allocate memory for tokenizer\n");
        goto err_cleanup;
    }
    self->m_index = 0;
    self->m_src = strdup(src);
    if (self->m_src == NULL) {
        perror("Failed to duplicate source string\n");
        goto err_cleanup;
    }
    self->code_lines_processed = 0;
    self->total_code_lines_processed = 0;
    return self;

err_cleanup:

    if (self != NULL)
        free(self);
    return NULL;
}

void t_free(struct tokenizer *self)
{
    if (self == NULL)
        return;

    if (self->m_src != NULL)
        free(self->m_src);

    free(self);
}

/**
 * Returns `'\0'`(null character) or actual peeked char at offset (default is 0)
 */
static char t_peek(const struct tokenizer *self, int offset)
{
    if (self->m_index + offset >= (int)strlen(self->m_src))
        return '\0';

    return self->m_src[self->m_index + offset];
}

/**
 * Return `char` at `self->m_index` and increment `self->m_index` by 1
 * - Assertion context to protect against maintenace
 * - Also check if garbage value is not returned
 */
static char t_consume(struct tokenizer *self)
{
    assert(self->m_src[(self->m_index)] && "Is validated by peek caller");
    assert(
        (self->m_index + 1 <= strlen(self->m_src))
        && "Does not exceed source bounds");

    return self->m_src[(self->m_index)++];
}

struct token *t_tokenize(struct tokenizer *self, int *token_count)
{
    *token_count = 0;
    size_t m_src_length = strlen(self->m_src);
    size_t size = m_src_length * sizeof(struct token);

    struct token *tokens = (struct token *)malloc(size);
    if (tokens == NULL) {
        perror("Failed to allocate memory for tokens while tokenizing\n");
        return NULL;
    }

    char buf[m_src_length + 1];
    int ofst = 0;
    size_t i = 0;

    while (t_peek(self, ofst) != '\0') {
        if (isalpha(t_peek(self, ofst))) {
            buf[i++] = t_consume(self);
            while (t_peek(self, ofst) != '\0' && isalnum(t_peek(self, ofst))) {
                buf[i++] = t_consume(self);
            }
            buf[i] = '\0'; // Null-terminate the token buffer
            if (strcmp(buf, "exit") == 0) {
                tokens[(*token_count)++] = (struct token) { .type = TEXIT };
                buf_clear(buf, &i);
            }
            else if (strcmp(buf, "let") == 0) {
                tokens[(*token_count)++] = (struct token) { .type = TLET };
                buf_clear(buf, &i);
            }
            else {
                (tokens[(*token_count)++]
                 = (struct token) { .type = TIDENT, .value = strdup(buf) });
                buf_clear(buf, &i);
            }
        }
        else if (isdigit(t_peek(self, ofst))) {
            buf[i++] = t_consume(self);
            while (t_peek(self, ofst) != '\0' && isdigit(t_peek(self, ofst))) {
                buf[i++] = t_consume(self);
            }
            buf[i] = '\0';
            (tokens[(*token_count)++]
             = (struct token) { .type = TINT_LIT, .value = strdup(buf) });
            buf_clear(buf, &i);
        }
        else if (t_peek(self, ofst) == '(') {
            t_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TPAREN_OPEN });
        }
        else if (t_peek(self, ofst) == ')') {
            t_consume(self);
            (tokens[(*token_count)++]
             = (struct token) { .type = TPAREN_CLOSE });
        }
        /* TODO: comments */
        else if (t_peek(self, ofst) == '/' && t_peek(self, 1) == '/') {
            assert(t_consume(self) == '/');
            assert(t_consume(self) == '/');
            while (isspace(t_peek(self, ofst))) {
                t_consume(self);
            }
            while (t_peek(self, ofst) != '\n') {
                buf[i++] = t_consume(self);
            }
            buf[i] = '\0';
            self->total_code_lines_processed += 1;

            (tokens[(*token_count)++]
             = (struct token) { .type = TCOMMENT, .value = strdup(buf) });
            buf_clear(buf, &i);
        }
        /* end of statement with line end `\n` (or semicolon in C) */
        else if (
            t_peek(self, ofst) == '\n' && (*token_count > 0)
            && tokens[(*token_count - 1)].type != TCOMMENT) { // == ';') {
            t_consume(self);
            tokens[(*token_count)++] = (struct token) { .type = TSEMICOLON };
            self->code_lines_processed += 1;
            self->total_code_lines_processed += 1;

            while (t_peek(self, ofst) == '\n') {
                assert(t_consume(self) == '\n');
                self->total_code_lines_processed += 1;
            }
            continue;
        }
        else if (t_peek(self, ofst) == ':') {
            t_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TCOLON });
        }
        else if (t_peek(self, ofst) == '=') {
            t_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TEQUAL });
        }
        else if (isspace(t_peek(self, ofst))) {
            t_consume(self);
        }
        else {
            perror("You messed up while peeking!\n");
            exit(EXIT_FAILURE);
        }
        if (i > m_src_length) {
            perror("Temporary buffer stack overflowed while tokenizing\n");
            exit(EXIT_FAILURE);
        }
    }
    self->m_index = 0;
    return tokens;
}

#endif /* CF17C85F_D892_43F8_B47B_0BA265F170E6 */
