#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define sizeof_field(t, f) (sizeof(((t *)0)->f))

#define USAGE_MESSAGE "Invalid usage. Correct usage is: ...\nxo <input.xo>\n"

/* =============================================================================
REGION_START: tokenizer.c */

// clang-format off

/*
 * ETokenType
 */
enum ETokenType { TOK_EXIT, TOK_INT_LIT, TOK_SEMI, TOK_PAREN_OPEN, TOK_PAREN_CLOSE, TOK_IDENT, TOK_LET, TOK_EQ, };
// clang-format on

char *tokentype_to_str(enum ETokenType self)
{
    switch (self) {
    case TOK_EXIT:
        return "TOK_EXIT";
    case TOK_INT_LIT:
        return "TOK_INT_LIT";
    case TOK_SEMI:
        return "TOK_SEMI";
    case TOK_PAREN_OPEN:
        return "TOK_PAREN_OPEN";
    case TOK_PAREN_CLOSE:
        return "TOK_PAREN_CLOSE";
    case TOK_IDENT:
        return "TOK_IDENT";
    case TOK_LET:
        return "TOK_LET";
    case TOK_EQ:
        return "TOK_EQ";
    default:
        perror("Found invalid token type\n");
        return NULL; // FIXME: should this return NULL?
    }
}

/*
 * Token
 */
struct Token {
    enum ETokenType type;
    char *value;
};

/* NOTE: TEMPORARY FUNCTION skips parsing and generation */
char *token_array_to_asm(struct Token self[], size_t token_count)
{
    char output[1000];

    for (int i = 0; i < token_count; i++) {
        printf("Token %d: %u %s\n", i, self[i].type, self[i].value);

        if (self[i].type == TOK_EXIT) {
            // TODO: extract from main
        }
    }
    return "\0";
}

void token_array_destroy(struct Token *self, int token_count)
{
    if (self != NULL) {
        for (int i = 0; i < token_count; i++) {
            printf("Token %d: %u %s %s\n", i, self[i].type, tokentype_to_str(self[i].type), self[i].value);
            free(self[i].value);
        }
        free(self);
    }
}

/*
 * Tokenizer
 */
struct Tokenizer {
    char *m_src;
    size_t m_index;
};

struct Tokenizer *tokenizer_create(const char *src)
{
    struct Tokenizer *self = (struct Tokenizer *)malloc(sizeof(struct Tokenizer));
    if (self == NULL) {
        perror("Failed to allocate memory for Tokenizer\n");
        return NULL;
    }

    self->m_index = 0;
    self->m_src = strdup(src);
    if (self->m_src == NULL) {
        perror("Failed to duplicate source string\n");
        free(self); // Clean up partially allocated struct
        return NULL;
    }
    return self;
}

void tokenizer_destroy(struct Tokenizer *self)
{
    if (self != NULL)
        free(self->m_src);

    free(self);
}

// Returns `'\0'`(null character) or actual peeked char at offset (default 0)
static char tokenizer_peek(const struct Tokenizer *self, int offset)
{
    if (self->m_index + offset >= strlen(self->m_src))
        return '\0';

    return self->m_src[self->m_index + offset];
}

// Return `char` at `self->m_index` and increment `self->m_index` by 1
// Assertion context to protect against maintenace
// - Also check if garbage value is not returned
static char tokenizer_consume(struct Tokenizer *self)
{
    assert(self->m_src[(self->m_index)] && "Is validated by peek caller");
    assert((self->m_index + 1 <= strlen(self->m_src)) && "Does not exceed source bounds");

    return self->m_src[(self->m_index)++];
}

int buf_clear(char array[], int *array_length)
{
    if (array == NULL)
        return -EBUSY; // see linux style guide

    for (int i = 0; i < *array_length; i++) {
        array[i] = '\0';
    }
    *array_length = 0;

    return 0; // success
}

struct Token *tokenizer_tokenize(struct Tokenizer *self, int *token_count)
{
    *token_count = 0;
    size_t LOOP_LIMIT = 1000;
    size_t m_src_length = strlen(self->m_src);

    char buf[m_src_length + 1];
    struct Token *tokens = (struct Token *)malloc(m_src_length * sizeof(struct Token));
    if (tokens == NULL) {
        perror("Failed to allocate memory for tokens while tokenizing\n");
        return NULL;
    }

    int offset = 0;
    int i = 0;

    while (tokenizer_peek(self, offset) != '\0') {
        if (isalpha(tokenizer_peek(self, offset))) {
            buf[i++] = tokenizer_consume(self);
            while (tokenizer_peek(self, offset) != '\0' && isalnum(tokenizer_peek(self, offset))) {
                buf[i++] = tokenizer_consume(self);
            }
            buf[i] = '\0'; // Null-terminate the token buffer
            if (strcmp(buf, "exit") == 0) {
                tokens[(*token_count)++] = (struct Token) { .type = TOK_EXIT };
                buf_clear(buf, &i);
            } else if (strcmp(buf, "let") == 0) {
                tokens[(*token_count)++] = (struct Token) { .type = TOK_LET };
                buf_clear(buf, &i);
            } else {
                (tokens[(*token_count)++] = (struct Token) { .type = TOK_IDENT, .value = strdup(buf) });
                buf_clear(buf, &i);
            }
        } else if (isdigit(tokenizer_peek(self, offset))) {
            buf[i++] = tokenizer_consume(self);
            while (tokenizer_peek(self, offset) != '\0' && isdigit(tokenizer_peek(self, offset))) {
                buf[i++] = tokenizer_consume(self);
            }
            buf[i] = '\0';
            (tokens[(*token_count)++] = (struct Token) { .type = TOK_INT_LIT, .value = strdup(buf) });
            buf_clear(buf, &i);
        } else if (tokenizer_peek(self, offset) == '(') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct Token) { .type = TOK_PAREN_OPEN });
        } else if (tokenizer_peek(self, offset) == ')') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct Token) { .type = TOK_PAREN_CLOSE });
        } else if (tokenizer_peek(self, offset) == ';') {
            tokenizer_consume(self);
            tokens[(*token_count)++] = (struct Token) { .type = TOK_SEMI };
        } else if (tokenizer_peek(self, offset) == '=') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct Token) { .type = TOK_EQ });
        } else if (isspace(tokenizer_peek(self, offset))) {
            tokenizer_consume(self);
        } else {
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

/* REGION_END: tokenizer.c
============================================================================= */

/* =============================================================================
REGION_START: parser.c */

//
struct NodeExprIntLit {
    struct Token int_lit;
};

struct NodeExprIdent {
    struct Token ident;
};

enum NodeExprType { NODE_EXPR_INT_LIT, NODE_EXPR_IDENT };

struct NodeExpr {
    enum NodeExprType type; // variant type
    union {
        struct NodeExprIntLit int_lit_expr;
        struct NodeExprIdent ident_expr;
    } var; // variant
};

struct NodeStmtExit {
    struct NodeExpr expr;
};

struct NodeStmtLet {
    struct Token ident;
    struct NodeExpr expr;
};

enum NodeStmtType { NODE_STMT_EXIT, NODE_STMT_LET };

struct NodeStmt {
    enum NodeStmtType type; // variant type
    union {
        struct NodeStmtExit exit_stmt;
        struct NodeStmtLet let_stmt;
    } var; // variant
};

struct NodeProg {
    struct NodeStmt *stmts;
    size_t stmt_count;
};

/*
 * Parser
 */
struct Parser {
    struct Token *m_tokens;
    size_t m_token_count;
    size_t m_index;
};

// NOTE: Lifetime of `tokens` remains till asm is generated and written to
//       file. Do not allocate or free it anywhere in `Parser`.
struct Parser *parser_create(struct Token *tokens, const size_t token_count)
{
    struct Parser *self = (struct Parser *)(malloc(token_count * sizeof(struct Parser)));
    if (self == NULL) {
        perror("Failed to allocate memory for Parser\n");
        // FIXME: panic if malloc returns NULL, do not return NULL!!
        return NULL;
    }

    if (tokens == NULL || token_count == 0) {
        perror("Empty tokens array passed while creating parser\n");
        self->m_tokens = NULL;
    } else {
        self->m_tokens = tokens;
    }

    self->m_index = 0;
    self->m_token_count = token_count;
    return self;
}

void parser_destroy(struct Parser *self)
{
    if (self != NULL)
        free(self);
}

static struct Token *parser_peek(const struct Parser *self, int offset)
{
    if (self->m_index + offset >= self->m_token_count)
        return NULL;

    return &self->m_tokens[self->m_index + offset];
}

static struct Token parser_consume(struct Parser *self)
{
    return self->m_tokens[(self->m_index)++];
}

struct NodeExpr *parser_parse_expr(struct Parser *self)
{
    return NULL;
}

struct NodeStmt *parser_parse_stmt(struct Parser *self)
{
    return NULL;
}

struct NodeProg *parser_parse_prog(struct Parser *self)
{
    struct NodeProg *prog;
    {
        int OFFSET = 0;
        while (parser_peek(self, OFFSET) != NULL) {
            struct NodeStmt *stmt = parser_parse_stmt(self);
            if (stmt != NULL) {
                {
                    // TODO
                }
                return NULL;
            } else {
                fprintf(stderr, "In function `parser_parse_prog`#while#if 1(%d):\n", __LINE__);
                fprintf(stderr, "[%d]: error: Invalid statement\n", __LINE__);
                return NULL;
            }
            return NULL;
        }
    }
    return prog;
}

/* REGION_END: parser.c
============================================================================= */

/*
 * main entrypoint
 */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, USAGE_MESSAGE);
        return EXIT_FAILURE;
    }

    // File I/O

    char *filename = argv[1]; // TODO: err if extension is not `.xo`
    FILE *input_file_xo = fopen(filename, "r");
    if (input_file_xo == NULL) {
        perror("Could not open input file\n");
        return EXIT_FAILURE;
    }

    fseek(input_file_xo, 0, SEEK_END);
    long input_size = ftell(input_file_xo);
    rewind(input_file_xo);

    char buffer[input_size + 1]; // +1 for the null terminator
    if (fread(&buffer, sizeof(char), input_size, input_file_xo) != input_size) {
        perror("Failed to read input file to buffer/n");
        return EXIT_FAILURE;
    }
    buffer[input_size] = '\0'; // `strdup` relies on null -terminated strings
    fclose(input_file_xo);

    // MODULE::tokenizer.c

    int token_count = 0;
    struct Tokenizer *tokenizer = tokenizer_create(buffer);
    struct Token *tokens = tokenizer_tokenize(tokenizer, &token_count); // Token[] allocate
    tokenizer_destroy(tokenizer);

    // MODULE::parser.c

    struct Parser *parser = parser_create(tokens, token_count);
    struct NodeProg *prog = parser_parse_prog(parser);
    if (prog == NULL) {
        parser_destroy(parser);
        token_array_destroy(tokens, token_count);
        fprintf(stderr, "Invalid program\n");
        return EXIT_FAILURE;
    }

    // MODULE::generation.c

    {
        // struct Generator *generator = generator_create(prog);
        // char *output = generator_generate_prog(generator);

        // //...OR
        // { // C++
        //     std::fstream file("out.asm", std::ios::out);
        //     file << generator.gen_prog(); // OR file << output // inline function here
        // }

        parser_destroy(parser);
    }

    //
    // system(...);
    // system(...);
    //
    // return EXIT_STATUS;
    // // ...EOF

    char *output_asm = NULL; // Dynamic buffer
    {
        // SUBMODULE::generater.c::tokens_to_asm REFACTOR: to function...

        static size_t MAX_ASM_SIZE = 10000; // PERF: generate required memory based on input file
        size_t len_token = token_count;

        output_asm = (char *)malloc(MAX_ASM_SIZE * sizeof(char)); // adjust size as needed
        if (output_asm == NULL) {
            fprintf(stderr, "Failed to allocate memory to assembly output\n");
            exit(EXIT_FAILURE); // return NULL; // if in a function
        }
        output_asm[0] = '\0'; // initialize buffer
        strcat(output_asm, "global _start\n_start:\n");

        for (int i = 0; i < len_token; i++) {
            if (tokens[i].type == TOK_EXIT) {
                if (i + 1 < len_token && tokens[i + 1].type == TOK_INT_LIT) {
                    if (i + 2 < len_token && tokens[i + 2].type == TOK_SEMI) {
                        strcat(output_asm, "    mov rax, 60\n"); // exit code
                        strcat(output_asm, "    mov rdi, "); // register expr
                        char int_lit_str[20];
                        sprintf(int_lit_str, "%s", tokens[i + 1].value); // register no.
                        strcat(output_asm, int_lit_str);
                        strcat(output_asm, "\n");
                        strcat(output_asm, "    syscall\n");
                    }
                }
            }
        }
        token_array_destroy(tokens, token_count); // Token[] cleanup
    }
    // TODO: handle error first
    if (output_asm != NULL) {
        printf("Generated Assembly Code:\n%s\n", output_asm);
        FILE *file_asm = fopen("out.asm", "w");
        if (file_asm == NULL) {
            perror("Could not open file\n");
            return EXIT_FAILURE;
        }
        fputs(output_asm, file_asm);
        fclose(file_asm);
        free(output_asm);
        output_asm = NULL;
    }

    // System shell commands

    system("nasm -felf64 out.asm"); // assemble and...
    system("ld out.o -o out"); // link the assembly code

    return EXIT_SUCCESS;
}

// /*
//  * ErrorOptionType
//  */
// enum OptionErrorType {
//     OPTION_SUCCESS,
//     OPTION_ERROR_NONE,
//     OPTION_ERROR_INVALID,
//     // ...
// };
//
// NOTE: free the memory with option_free(...)
// static struct Option parser_peek_optalloc(const struct Parser *self, int offset)
// {
//     if (self->m_index + offset >= self->m_token_count)
//         return option_none();
//
//     size_t token_size = sizeof(self->m_tokens[self->m_index + offset].value);
//
//     struct Token *token = malloc(token_size * sizeof(char));
//     token = &self->m_tokens[self->m_index + offset];
//     return option_some(token, token_size);
// }
//
// // clang-format off
// /*
//  * OptionInt
//  */
// struct OptionInt { bool has_value; int value; };
// struct OptionInt option_int_some(int value) { return (struct OptionInt) { .has_value = true, .value = value }; }
// struct OptionInt option_int_none() { return (struct OptionInt) { .has_value = false }; }
// bool option_int_has_value(const struct OptionInt *self) { return self->has_value; }
// int option_int_value(const struct OptionInt *self) { if (!self->has_value) { fprintf(stderr, "Unwrapped a None
// optional OptionInt\n"); exit(EXIT_FAILURE); } return self->value; }
// // clang-format on
// /*
//  * Option
//  */
// struct Option {
//     bool has_value;
//     void *value;
// };
// struct Option option_some(const void *value, size_t size)
// {
//     struct Option opt;
//     opt.has_value = true;
//     opt.value = malloc(size);
//     if (!opt.value) {
//         fprintf(stderr, "Failed to allocate memory for Option value\n");
//         exit(EXIT_FAILURE);
//     }
//     memcpy(opt.value, value, size);
//     return opt;
// }
// struct Option option_none()
// {
//     return (struct Option) { .has_value = false, .value = NULL };
// }
// bool option_has_value(const struct Option *self)
// {
//     return self->has_value;
// }
// const void *option_value(const struct Option *self)
// {
//     if (!self->has_value) {
//         fprintf(stderr, "Unwrapped a None optional Option\n");
//         exit(EXIT_FAILURE);
//     }
//     return self->value;
// }
// void option_free(struct Option *self)
// {
//     if (self->has_value) {
//         free(self->value);
//     }
// }
// int try_option_example()
// {
//     int some_value = 42;
//     struct Option some_option = option_some(&some_value, sizeof(int));
//     if (option_has_value(&some_option)) {
//         printf("Option has a value: %d\n", *(int *)option_value(&some_option));
//     } else {
//         printf("Option does not have a value.\n");
//     }
//     const int *val = option_value(&some_option);
//     struct Option none_option = option_none();
//     if (option_has_value(&none_option)) {
//         printf("None Option has a value.\n");
//     } else {
//         printf("None Option does not have a value.\n");
//     }
//     option_free(&some_option);
//     option_free(&none_option);
//     return 0;
// }
//
