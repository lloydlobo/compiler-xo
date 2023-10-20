/*
 *  lumina/src/main.c
 */

#include <asm-generic/errno-base.h>
#include <assert.h>
#include <bits/types/clock_t.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "err.h"
#include "generator.h"
#include "lexer.h"
#include "lumlib.h"
#include "parser.h"

#define USAGE_MESSAGE "Invalid usage. Correct usage is: ...\nlumina <input.lum>\n"

struct profiler_timer {
        double time_start;
        double time_end;
        double time_elapsed;
};
void profiler_timer_elapse(struct profiler_timer *self) {
        self->time_elapsed += ((double)(self->time_end - self->time_start)) / CLOCKS_PER_SEC;
};
struct profiler {
        struct {
                size_t number;
                char  *label;
        } workspace;
        struct {
                size_t code_lines_processed;
                size_t total_lines_processed;
                size_t tokens_processed;
        } lexer;
        // perform_loads for w2 took 22ms.
        struct profiler_timer fileio;
        struct profiler_timer front_end; // x64 output time + x64 linking time
        // struct profiler_timer back_end; //[x64] llvm?
        struct profiler_timer x64;
        struct profiler_timer compiler;
        struct profiler_timer linker;
        struct profiler_timer total;
};
struct profiler profiler_init(size_t workspace_number, char *workspace_label) {
        struct profiler self;

        memset(&self, 0, sizeof(struct profiler));

        self.workspace.label = workspace_label;
        self.workspace.number = workspace_number;

        return self;
}
void profiler_start(struct profiler *self) {
        self->total.time_start = clock();
};
void profiler_stop(struct profiler *self) {
        self->total.time_end = clock();

        profiler_timer_elapse(&self->total);
        profiler_timer_elapse(&self->linker);
        profiler_timer_elapse(&self->compiler);
        profiler_timer_elapse(&self->x64);
        profiler_timer_elapse(&self->front_end);
};
void profiler_emit(const struct profiler *self) {
        double acc_elapsed_time = self->front_end.time_elapsed + self->x64.time_elapsed +
                                  self->compiler.time_elapsed + self->linker.time_elapsed;

        printf(
            "\nStats for Workspace %zu (\"%s\"):\n", self->workspace.number, self->workspace.label);
        printf(
            "Lexer lines processed: %zu (%zu including blanklines, "
            "comments.)\n",
            self->lexer.code_lines_processed,
            self->lexer.total_lines_processed);
        printf("Lexer tokens processed: %zu.\n", self->lexer.tokens_processed);
        printf("Front-end time: %.6f seconds.\n", self->front_end.time_elapsed);
        printf("x64       time: %.6f seconds.\n\n", self->x64.time_elapsed);
        printf("Compiler  time: %.6f seconds.\n", self->compiler.time_elapsed);
        printf("Link      time: %.6f seconds.\n", self->linker.time_elapsed);
        printf(
            "Total     time: %.6f seconds. (%.6f seconds for above.)\n\n",
            self->total.time_elapsed,
            acc_elapsed_time);
};

err_t parse_input_file_ext(const char *input, const char *expect_ext) {
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

/**
 * main entrypoint
 */
int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, USAGE_MESSAGE);
                return EXIT_FAILURE;
        }

        struct profiler profile;

        profile = profiler_init(1, "Debug");
        profiler_start(&profile);
        /* read input source file */

        profile.front_end.time_start = clock();
        const char *filename = argv[1];
        const char *expect_ext = "lum";
        err_t       err = parse_input_file_ext(filename, expect_ext);
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
                if (fread(&contents, sizeof(char), input_size, input_file_lum) != input_size) {
                        perror("error: Failed to read input file to buffer/n");
                        fclose(input_file_lum);
                        return EXIT_FAILURE;
                }
                contents[input_size] = '\0';
                fclose(input_file_lum);
        }
        profile.front_end.time_end = clock();
        /* module::lexer.c */

        profile.compiler.time_start = clock();
        int           mut_token_count = 0;
        struct lexer *lexer = lexer_init(contents);
        struct token *tokens = lexer_tokenize(lexer, &mut_token_count);
        profile.lexer.total_lines_processed = lexer->total_code_lines_processed;
        profile.lexer.code_lines_processed = lexer->code_lines_processed;
        profile.lexer.tokens_processed = (size_t)mut_token_count;
        lexer_free(lexer);
        /* module::parser.c */

        struct parser    *parser = parser_init(tokens, mut_token_count);
        struct node_prog *prog = parser_parse_prog(parser);
        if (prog == NULL)
                goto fail_clean_parser_tokens;
        p_node_prog_print(prog);
        /* module::generator.c */

        struct generator generator = generator_init(prog);
        char            *output_asm = generator_gen_prog(&generator);
        if (prog != NULL) { // TODO: free prog after generator?
                if (prog->stmts != NULL)
                        free(prog->stmts);
                free(prog);
        }
        if (output_asm == NULL)
                goto fail_clean_generator;

        generator_free(&generator);
        parser_free(parser);
        token_free_tokens(tokens, mut_token_count);
        profile.compiler.time_end = clock();
        /* write assembly to output asm file */

        printf("\nGenerated Assembly Code:\n\n%s\n", output_asm);

        profiler_timer_elapse(&profile.front_end); // record last
        profile.front_end.time_start = clock();
        {
                FILE *file_out_asm = fopen("out.asm", "w");
                if (file_out_asm == NULL)
                        goto fail_clean_file_out_asm_output_asm;
                fputs(output_asm, file_out_asm);
                fclose(file_out_asm);
        }
        profile.front_end.time_end = clock();
        /* assemble and... link the assembly code */

        profile.x64.time_start = clock();
        system("nasm -felf64 out.asm");
        profile.x64.time_end = clock();

        profile.linker.time_start = clock();
        system("ld out.o -o out");
        profile.linker.time_end = clock();

        profiler_stop(&profile);
        profiler_emit(&profile);

        return EXIT_SUCCESS;

fail_clean_file_out_asm_output_asm:
        perror("Could not open/create asm output file\n");
        free(output_asm);

        return EXIT_FAILURE;

fail_clean_generator:
        perror("Could not generate assembly output\n");
        generator_free(&generator);

        return EXIT_FAILURE;

fail_clean_parser_tokens:
        fprintf(stderr, "--%d-- Invalid program\n", __LINE__);
        parser_free(parser);
        token_free_tokens(tokens, mut_token_count);

        return EXIT_FAILURE;
}
