/*
 *  lumina/src/main.c
 */

#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "generator.h"
#include "hashtable.h"
#include "lumlib.h"
#include "parser.h"
#include "print.h"
#include "tokenizer.h"

#define OUT_ASM_FILENAME "out.asm"
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define sizeof_field(t, f) (sizeof(((t *)0)->f))

#define USAGE_MESSAGE \
    "Invalid usage. Correct usage is: ...\nlumina <input.lum>\n"

err_t parse_input_file_ext(const char *input, const char *expect_ext);

err_t parse_input_file_ext(const char *input, const char *expect_ext)
{
    if (input == NULL || expect_ext == NULL)
        return ErrInvalid;
    size_t input_len = strlen(input), ext_len = strlen(expect_ext);
    if (input_len < ext_len + 1)
        return ErrBadName;
    const char *ext_check = input + (input_len - ext_len);
    if (strcmp(ext_check, expect_ext) != 0)
        return ErrBadfd;

    return ErrOk;
}

/*
 * main entrypoint
 */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, USAGE_MESSAGE);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *expect_ext = "lum";
    err_t err = parse_input_file_ext(filename, expect_ext);
    if (err != ErrOk) {
        fprintf(stderr, "error: %s: Expected `<filename>.lum`\n", err_str(err));
        return EXIT_FAILURE;
    };
    FILE *input_file_lum = fopen(filename, "r");
    if (input_file_lum == NULL) {
        perror("error: Could not open input file\n");
        return EXIT_FAILURE;
    }
    fseek(input_file_lum, 0, SEEK_END);
    long input_size = ftell(input_file_lum);
    rewind(input_file_lum);

    char contents[input_size + 1]; // +1 for the null terminator
    {
        if (fread(&contents, sizeof(char), input_size, input_file_lum)
            != input_size) {
            perror("error: Failed to read input file to buffer/n");
            fclose(input_file_lum);
            return EXIT_FAILURE;
        }
        contents[input_size] = '\0';
        fclose(input_file_lum);
    }

    println("Hello, World from Lumina!");

    /* module::tokenizer.c */

    int mut_token_count = 0;
    struct tokenizer *tokenizer = t_init(contents);
    struct token *tokens = t_tokenize(tokenizer, &mut_token_count);
    t_free(tokenizer);

    /* module::parser.c */

    struct parser *parser = p_init(tokens, mut_token_count);
    struct node_prog *prog = p_parse_prog(parser);
    if (prog == NULL)
        goto err_clean_parser_tokens;
    p_node_prog_print(prog);

    /* module::generator.c */

    struct generator generator = g_init(prog);
    char *output_asm = g_gen_prog(&generator);
    // generator_free(generator);
    if (prog != NULL) {
        if (prog->stmts != NULL)
            free(prog->stmts);
        free(prog);
    }
    if (output_asm == NULL)
        goto err_clean_generator;

    p_free(parser);
    token_free_tokens(tokens, mut_token_count);

    {
        printf("Generated Assembly Code:\n%s\n", output_asm);
        FILE *file_out_asm = fopen(OUT_ASM_FILENAME, "w");
        if (file_out_asm == NULL)
            goto err_clean_file_out_asm_output_asm;
        fputs(output_asm, file_out_asm);
        fclose(file_out_asm);
        // free(output_asm);
    }

    /* assemble and... link the assembly code */
    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    return EXIT_SUCCESS;

err_clean_file_out_asm_output_asm:
    perror("Could not open/create asm output file\n");
    free(output_asm);
    return EXIT_FAILURE;

err_clean_generator:
    perror("Could not generate assembly output\n");
    // generator_free(generator);
    return EXIT_FAILURE;

err_clean_parser_tokens:
    fprintf(stderr, "--%d-- Invalid program\n", __LINE__);
    p_free(parser);
    token_free_tokens(tokens, mut_token_count);
    return EXIT_FAILURE;
}
