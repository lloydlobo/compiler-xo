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

// clang-format off
#define USAGE_MESSAGE "Invalid usage. Correct usage is: ...\nxo <input.xo>\n"
// clang-format on

/* =============================================================================
REGION_START: tokenizer.c */

/*
 * ETokenType
 */
enum TokenType {
    TOK_EXIT,
    TOK_INT_LIT,
    TOK_SEMICOLON,
    TOK_PAREN_OPEN,
    TOK_PAREN_CLOSE,
    TOK_SQUIRLY_OPEN,
    TOK_SQUIRLY_CLOSE,
    TOK_IDENT,
    TOK_LET,
    TOK_EQUAL,
};

char *tokentype_to_str(enum TokenType self)
{
    switch (self) {
    case TOK_EXIT:
        return "TOK_EXIT";
    case TOK_INT_LIT:
        return "TOK_INT_LIT";
    case TOK_SEMICOLON:
        return "TOK_SEMICOLON";
    case TOK_PAREN_OPEN:
        return "TOK_PAREN_OPEN";
    case TOK_PAREN_CLOSE:
        return "TOK_PAREN_CLOSE";
    case TOK_SQUIRLY_OPEN:
        return "TOK_SQUIRLY_OPEN";
    case TOK_SQUIRLY_CLOSE:
        return "TOK_SQUIRLY_CLOSE";
    case TOK_IDENT:
        return "TOK_IDENT";
    case TOK_LET:
        return "TOK_LET";
    case TOK_EQUAL:
        return "TOK_EQ";
    default:
        perror("Found invalid token type\n");
        return NULL; // FIXME: should this return NULL?
    }
}

/**
 * Token
 */
struct token {
    enum TokenType type;
    char *value;
};

// NOTE : free after this scope token_array_free(tokens, token_count); // Token[] cleanup
/* NOTE: TEMPORARY FUNCTION skips parsing and generation */
char *token_array_to_asm(struct token self[], size_t token_count)
{
    char output[1000];

    for (int i = 0; i < token_count; i++) {
        printf("--%d-- Token %d: %u %s\n", __LINE__, i, self[i].type, self[i].value);

        if (self[i].type == TOK_EXIT) {
            // char *output_asm = NULL; // Dynamic buffer
            // {
            //     // SUBMODULE::generater.c::tokens_to_asm REFACTOR: to function...
            //
            //     static size_t MAX_ASM_SIZE = 10000; // PERF: generate required memory based on input file
            //     size_t len_token = token_count;
            //
            //     output_asm = (char *)malloc(MAX_ASM_SIZE * sizeof(char)); // adjust size as needed
            //     if (output_asm == NULL) {
            //         fprintf(stderr, "Failed to allocate memory to assembly output\n");
            //         exit(EXIT_FAILURE); // return NULL; // if in a function
            //     }
            //     output_asm[0] = '\0'; // initialize buffer
            //     strcat(output_asm, "global _start\n_start:\n");
            //
            //     for (int i = 0; i < len_token; i++) {
            //         if (tokens[i].type == TOK_EXIT) {
            //             if (i + 1 < len_token && tokens[i + 1].type == TOK_INT_LIT) {
            //                 if (i + 2 < len_token && tokens[i + 2].type == TOK_SEMICOLON) {
            //                     strcat(output_asm, "    mov rax, 60\n"); // exit code
            //                     strcat(output_asm, "    mov rdi, "); // register expr
            //                     char int_lit_str[20];
            //                     sprintf(int_lit_str, "%s", tokens[i + 1].value); // register no.
            //                     strcat(output_asm, int_lit_str);
            //                     strcat(output_asm, "\n");
            //                     strcat(output_asm, "    syscall\n");
            //                 }
            //             }
            //         }
            //     }
            // }
        }
    }
    return "\0";
}

void token_array_free(struct token *self, int token_count)
{
    if (self != NULL) {
        for (int i = 0; i < token_count; i++) {
            printf(
                "--%d-- Token %d: %u %s %s\n",
                __LINE__,
                i,
                self[i].type,
                tokentype_to_str(self[i].type),
                self[i].value);

            free(self[i].value);
        }
        free(self);
    }
}

/**
 * Tokenizer
 */
struct tokenizer {
    char *m_src;
    size_t m_index;
};

struct tokenizer *tokenizer_init(const char *src)
{
    struct tokenizer *self = (struct tokenizer *)malloc(sizeof(struct tokenizer));
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

void tokenizer_free(struct tokenizer *self)
{
    if (self != NULL)
        free(self->m_src);

    free(self);
}

// Returns `'\0'`(null character) or actual peeked char at offset (default 0)
static char tokenizer_peek(const struct tokenizer *self, int offset)
{
    if (self->m_index + offset >= strlen(self->m_src))
        return '\0';

    return self->m_src[self->m_index + offset];
}

// Return `char` at `self->m_index` and increment `self->m_index` by 1
// Assertion context to protect against maintenace
// - Also check if garbage value is not returned
static char tokenizer_consume(struct tokenizer *self)
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

struct token *tokenizer_tokenize(struct tokenizer *self, int *token_count)
{
    *token_count = 0;
    size_t LOOP_LIMIT = 1000;
    size_t m_src_length = strlen(self->m_src);

    char buf[m_src_length + 1];
    struct token *tokens = (struct token *)malloc(m_src_length * sizeof(struct token));
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
                tokens[(*token_count)++] = (struct token) { .type = TOK_EXIT };
                buf_clear(buf, &i);
            } else if (strcmp(buf, "let") == 0) {
                tokens[(*token_count)++] = (struct token) { .type = TOK_LET };
                buf_clear(buf, &i);
            } else {
                (tokens[(*token_count)++] = (struct token) { .type = TOK_IDENT, .value = strdup(buf) });
                buf_clear(buf, &i);
            }
        } else if (isdigit(tokenizer_peek(self, offset))) {
            buf[i++] = tokenizer_consume(self);
            while (tokenizer_peek(self, offset) != '\0' && isdigit(tokenizer_peek(self, offset))) {
                buf[i++] = tokenizer_consume(self);
            }
            buf[i] = '\0';
            (tokens[(*token_count)++] = (struct token) { .type = TOK_INT_LIT, .value = strdup(buf) });
            buf_clear(buf, &i);
        } else if (tokenizer_peek(self, offset) == '(') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TOK_PAREN_OPEN });
        } else if (tokenizer_peek(self, offset) == ')') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TOK_PAREN_CLOSE });
        } else if (tokenizer_peek(self, offset) == ';') {
            tokenizer_consume(self);
            tokens[(*token_count)++] = (struct token) { .type = TOK_SEMICOLON };
        } else if (tokenizer_peek(self, offset) == '=') {
            tokenizer_consume(self);
            (tokens[(*token_count)++] = (struct token) { .type = TOK_EQUAL });
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
struct node_expr_int_lit {
    struct token int_lit;
};

struct node_expr_ident {
    struct token ident;
};

enum NodeExprType { NODE_EXPR_INT_LIT, NODE_EXPR_IDENT };

struct node_expr {
    enum NodeExprType type; // variant type
    union {
        struct node_expr_int_lit *int_lit;
        struct node_expr_ident *ident;
    } var; // variant // FIXME: this seems anonymous (typedef vs global forwarding?)
};

struct node_stmt_exit {
    struct node_expr *expr;
};

struct node_stmt_let {
    struct token ident;
    struct node_expr *expr;
};

enum NodeStmtType { NODE_STMT_EXIT, NODE_STMT_LET };

struct node_stmt {
    enum NodeStmtType type; // variant type
    union {
        struct node_stmt_exit *exit;
        struct node_stmt_let *let;
    } var; // variant
};

struct node_prog {
    struct node_stmt **stmts;
    size_t stmt_count;
};

void parser_free_expr(struct node_expr *self);
void parser_free_stmt(struct node_stmt *self);
void parser_free_prog(struct node_prog *self);

/*
 * Parser
 */
struct parser {
    struct token *m_tokens;
    size_t m_token_count;
    size_t m_index;
    // Define ArenaAllocator if necessary
};

// Function prototypes for parser functions

struct node_expr *parser_parse_expr(struct parser *self);
struct node_stmt *parser_parse_stmt(struct parser *self);
struct node_prog *parser_parse_prog(struct parser *self);

static struct token *parser_peek(const struct parser *self, int offset);
static struct token *parser_consume(struct parser *self);
static struct node_expr *parser_parse_int_lit(struct parser *self);
static struct node_expr *parser_parse_ident(struct parser *self);
static struct node_stmt *parser_parse_exit_stmt(struct parser *self);
static struct node_stmt *parser_parse_let_stmt(struct parser *self);

static struct token *parser_peek(const struct parser *self, int offset)
{
    if (self->m_index + offset >= self->m_token_count)
        return NULL;

    return &self->m_tokens[self->m_index + offset];
}
static struct token *parser_consume(struct parser *self)
{
    return &self->m_tokens[(self->m_index)++];
}
// TODO:    static inline Token try_consume(TokenType type, const std::string& err_msg);
// TODO:    static inline std::optional<Token> try_consume(TokenType type);

struct parser *parser_init(struct token *tokens, const size_t token_count)
{
    struct parser *self = (struct parser *)(malloc(token_count * sizeof(struct parser)));
    if (self == NULL) {
        perror("Failed to allocate memory for Parser\n");
        return NULL; // FIXME: panic if malloc returns NULL, do not return NULL!!
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

void parser_free(struct parser *self)
{
    if (self != NULL)
        free(self);
}

struct node_expr *parser_parse_expr(struct parser *self)
{
    struct token *token_p = parser_peek(self, 0);
    if (token_p != NULL && token_p->type == TOK_INT_LIT) {
        struct token token_c = *parser_consume(self);
        enum NodeExprType net_neil = NODE_EXPR_INT_LIT;
        struct node_expr_int_lit neil = { .int_lit = token_c };
        struct node_expr expr = { net_neil, { &neil } };

        return &expr; // FIXME
    }
    return NULL;
}

struct node_stmt *parser_parse_stmt(struct parser *self)
{
    {
        // TODO
    }
    return NULL;
}

struct node_prog *parser_parse_prog(struct parser *self)
{
    struct node_prog *prog; // TODO: malloc in prog and free memory after calling this func
    {
        int OFFSET = 0;
        while (parser_peek(self, OFFSET) != NULL) {
            struct node_stmt *stmt = parser_parse_stmt(self);
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

void parser_free_expr(struct node_expr *self)
{
    if (self == NULL)
        return;

    switch (self->type) {
    case NODE_EXPR_INT_LIT:
        free(self->var.int_lit);
        break;
    case NODE_EXPR_IDENT:
        free(self->var.ident);
        break;
    }

    free(self); // TODO: free(self->var);
}

void parser_free_stmt(struct node_stmt *self)
{
    if (self == NULL)
        return;

    switch (self->type) {
    case NODE_STMT_EXIT:
        parser_free_expr(self->var.exit->expr);
        free(self->var.exit);
        break;
    case NODE_STMT_LET:
        parser_free_expr(self->var.let->expr);
        free(self->var.let);
        break;
    }

    free(self); // TODO: should we free(self->var) too?
}

void parser_free_prog(struct node_prog *self)
{
    if (self == NULL)
        return;

    for (int i = 0, n = self->stmt_count; i < n; i++) {
        free(&self->stmts[i]);
    }

    free(self->stmts);
    free(self);
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

    char *filename = argv[1]; // TODO: err if extension is not `.xo`
    FILE *input_file_xo = fopen(filename, "r");
    if (input_file_xo == NULL) {
        perror("Could not open input file\n");
        return EXIT_FAILURE;
    }

    fseek(input_file_xo, 0, SEEK_END);
    long input_size = ftell(input_file_xo);
    rewind(input_file_xo);

    char contents[input_size + 1]; // +1 for the null terminator
    {
        if (fread(&contents, sizeof(char), input_size, input_file_xo) != input_size) {
            perror("Failed to read input file to buffer/n");
            fclose(input_file_xo);
            return EXIT_FAILURE;
        }
        contents[input_size] = '\0'; // `strdup` relies on null -terminated strings
        fclose(input_file_xo);
    }

    /* module::tokenizer.c */

    int mut_token_count = 0;
    struct tokenizer *tokenizer = tokenizer_init(contents);
    struct token *tokens = tokenizer_tokenize(tokenizer, &mut_token_count);
    tokenizer_free(tokenizer);

    /* module::parser.c */

    struct parser *parser = parser_init(tokens, mut_token_count);
    struct node_prog *prog = parser_parse_prog(parser);
    if (prog == NULL)
        goto err_clean_parser_tokens;

    parser_free(parser);
    token_array_free(tokens, mut_token_count);

    /* module::generator.c */

    // struct generator *generator = generator_init(prog);
    // parser_free_prog(prog);

    // char *output_asm = generator_generate_prog(generator);
    // if (output_asm == NULL)
    //     goto err_clean_generator;

    // generator_free(generator);

    // {
    //     printf("Generated Assembly Code:\n%s\n", output_asm);
    //     FILE *file_out_asm = fopen("out.asm", "w");
    //     if (file_out_asm == NULL)
    //         goto err_clean_output_asm;

    //     fputs(output_asm, file_out_asm);
    //     fclose(file_out_asm);
    // }
    // free(output_asm);

    /* assemble and... link the assembly code */

    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    return EXIT_SUCCESS;

    // err_clean_output_asm:
    //     perror("Could not open/create asm output file\n");
    //     free(output_asm);
    //     return EXIT_FAILURE;
    //
    // err_clean_generator:
    //     perror("Could not generate assembly output\n");
    //     generator_free(generator);
    //     return EXIT_FAILURE;

err_clean_parser_tokens:
    fprintf(stderr, "--%d-- Invalid program\n", __LINE__);
    parser_free(parser);
    token_array_free(tokens, mut_token_count);
    return EXIT_FAILURE;
}
