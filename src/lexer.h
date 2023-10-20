#ifndef CF17C85F_D892_43F8_B47B_0BA265F170E6
#define CF17C85F_D892_43F8_B47B_0BA265F170E6

#include "lumlib.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // temp for strlen strcmp

/*
 * Define macros to convert enum values to strings
 */
#define ENUM_TO_STR(e) #e
#define CASE_ENUM_TO_STR(e) \
        case e: \
                return #e

/* TokenType */
enum Token_Kind {
        // —————————————————————————————————————————————————————————————————————
        // Mentally insert ASCII types here from 0: Jai uses similar technique

        TOKEN_BITWISE_XOR = '^',
        TOKEN_BITWISE_NOT = '~',
        TOKEN_BITWISE_AND = '&',
        TOKEN_BITWISE_OR = '|',

        //...

        TOKEN_COLON,
        TOKEN_PAREN_OPEN,
        TOKEN_PAREN_CLOSE,
        TOKEN_CURLY_OPEN,
        TOKEN_CURLY_CLOSE,
        TOKEN_ENDL,

        //...

        // Mentally inserted ASCII types above till 255
        // —————————————————————————————————————————————————————————————————————

        TOKEN_IDENT = 256,
        TOKEN_NUMBER = 257,
        TOKEN_STRING = 258,

        TOKEN_INT_LIT = 259,
        TOKEN_ISEQUAL = 264,    // but is it ascii `=`?
        TOKEN_ISNOTEQUAL = 265, // but is it ascii `=`?

        // TOKEN_ISEQUAL_FOR_SWITCH_STATEMENT = 280, // numbered enum ends here

        // TOKEN_DOUBLE_MINUS,
        // TOKEN_TRIPLE_MINUS,

        // TOKEN_KEYWORD_EACH,

        TOKEN_COMMENT, // this is two backslash `//`

        TOKEN_EXIT,
        TOKEN_LET,

        TOKEN_EOF,

        TOKEN_ERROR,
};

const char *token_type_to_str(enum Token_Kind self) {
        switch (self) {
                CASE_ENUM_TO_STR(TOKEN_BITWISE_XOR);
                CASE_ENUM_TO_STR(TOKEN_BITWISE_AND);
                CASE_ENUM_TO_STR(TOKEN_BITWISE_OR);

                CASE_ENUM_TO_STR(TOKEN_IDENT);
                CASE_ENUM_TO_STR(TOKEN_NUMBER);
                CASE_ENUM_TO_STR(TOKEN_STRING);

                CASE_ENUM_TO_STR(TOKEN_INT_LIT);
                CASE_ENUM_TO_STR(TOKEN_ISEQUAL);
                CASE_ENUM_TO_STR(TOKEN_ISNOTEQUAL);

                CASE_ENUM_TO_STR(TOKEN_ENDL);
                CASE_ENUM_TO_STR(TOKEN_COLON);
                CASE_ENUM_TO_STR(TOKEN_PAREN_OPEN);
                CASE_ENUM_TO_STR(TOKEN_PAREN_CLOSE);
                CASE_ENUM_TO_STR(TOKEN_CURLY_OPEN);
                CASE_ENUM_TO_STR(TOKEN_CURLY_CLOSE);

                CASE_ENUM_TO_STR(TOKEN_COMMENT);

                CASE_ENUM_TO_STR(TOKEN_EXIT);
                CASE_ENUM_TO_STR(TOKEN_LET);

                CASE_ENUM_TO_STR(TOKEN_EOF);

                CASE_ENUM_TO_STR(TOKEN_ERROR);
        }
        return "Unknown"; // Handle the case where self is not recognized.
}

/** token */
struct token {
        enum Token_Kind kind; // = TOKEN_ERROR

        int l0; // = 0
        int c0; // = 0

        int l1; // = -1
        int c1; // = -1

        union {
                struct atom       *name;
                unsigned long long integer_value; // u64
                f32                f32_value;
                f64                f64_value;
                struct {
                        u64 count;
                        u8 *data; // text as bytes
                } string_value;
        };

        u32  numeric_flags;       // = 0; if it's a number or a note
        bool ident_is_backticked; // = false; used to check ` in #directive

        u32   identifier_length;
        char *value; // TODO: use static buffer
        // unsigned_char token_scratch_buffer[MAX_TOKEN_LENGTH];
};

// —————————————————————————————————————————————————————————————————————————————————————
// INIT FREE

/* impl struct token */

void token_free_tokens(struct token *self, int token_count) {
        if (self == NULL)
                return;

        for (int i = 0; i < token_count; i++) { // clang-format off
        printf( "--%d-- Token %d: %u %s %s\n", __LINE__, i, self[i].kind,
            token_type_to_str(self[i].kind), self[i].value); // clang-format on
                if (self[i].value != NULL)
                        free(self[i].value);
        }
        free(self);
}

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE
// ---

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC
// ---

/**
 * lexer
 */

struct lexer {
        char  *m_src;
        size_t m_index;

        size_t code_lines_processed;
        size_t total_code_lines_processed;

        int current_line_number;                            // = 0;
        int current_external_file_error_report_line_number; // = 0;
        int current_character_index;                        // = 0;
};
//
// void lexer_init(struct parser *_parser); {
//     parser = _parser;
//     interp = parser->interp;
//     eof_token.type = TOKEN_END_OF_INPUT;
// }
//
// void set_input_from_string(struct newstring string);
// void set_input_from_file(void *file); // `(void *) is (FILE *) to avoid stdio
// ...peek_next_token
// ...eat_token
// ...get_last_token
// int current_line_number = 0;
// int current_external_file_error_report_line_number = 0;
// int current_character_index = 0;
//
// // placed here as while reporting errors only struct lexer is available
// Ast_Directive_Load *current_load_directive = NULL;
// Newstring current_path_name;
// Newstring current_file_name;
//

// —————————————————————————————————————————————————————————————————————————————————————
// INIT FREE

struct lexer *lexer_init(const char *src) {
        struct lexer *lex = (struct lexer *)malloc(sizeof(struct lexer));
        if (lex == NULL) {
                perror("Failed to allocate memory for tokenizer\n");
                goto err_cleanup;
        }

        lex->m_src = strdup(src);
        if (lex->m_src == NULL) {
                perror("Failed to duplicate source string\n");
                goto err_cleanup;
        }

        lex->m_index = 0;

        lex->code_lines_processed = 0;
        lex->total_code_lines_processed = 0;

        return lex;

err_cleanup:

        if (lex != NULL)
                free(lex);

        return NULL;
}

void lexer_free(struct lexer *self) {
        if (self == NULL)
                return;

        if (self->m_src != NULL)
                free(self->m_src);

        free(self);
}

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

/** This is used after m_index is updated with eat_char */
static char peek_next_character(const struct lexer *self) {
        // assert(self->input.data!=NULL);
        // assert(input.data!=NULL);
        assert(self->m_src != NULL);

        // if (input_cursor >= input.count)
        if (self->m_index >= (int)strlen(self->m_src))
                // return -1;
                return '\0';

        // int result = input.data[input_cursor];
        char result = self->m_src[self->m_index];
        return result;
}

/**
 * Returns `'\0'`(null character) or actual peeked char at
 * lookahead (+-offset) (default is 0)
 */
static char peek_character(const struct lexer *self, int lookahead) {
        if (self->m_index + lookahead >= (int)strlen(self->m_src))
                return '\0';

        return self->m_src[self->m_index + lookahead];
}

/**
 * Return `char` at `self->m_index` and increment `self->m_index` by 1
 * - Assertion context to protect against maintenace
 * - Also check if garbage value is not returned
 */
// jai::void char lexer_eat_token(struct lexer *self) {
static char eat_character(struct lexer *self) {
        assert(self->m_src[(self->m_index)] && "Is validated by peek caller");
        assert((self->m_index + 1 <= strlen(self->m_src)) && "Does not exceed source bounds");

        return self->m_src[(self->m_index)++];
}
static char get_last_character(struct lexer *self) {
        assert(self->m_index - 1 >= 0);
        assert((self->m_index + 1 <= strlen(self->m_src)) && "Does not exceed source bounds");

        return self->m_src[(self->m_index - 1)];
}

// ----------------------------------------------//
// ----------------- TODO ---------------------- //

enum Hex_Digit_Type {
        HEX_DIGIT_8BIT = 0,
        HEX_DIGIT_UNICODE_32 = 1,
        HEX_DIGIT_UNICODE_64 = 2,
};

// // TODO: import REPORT_NOLABEL from general lumlib or err.h
// // Taken from Jai lexer.cpp
// int lexer_get_hex_digit(struct lexer *lexer, enum Hex_Digit_Type type) {
//         assert(false && "unimplemented");
//
//         char c = peek_next_character(lexer);
//
//         if ((c >= 'a') && (c <= 'f')) {
//                 eat_character(lexer);
//                 return 10 + c - 'a';
//         } else if ((c >= 'A') && (c <= 'F')) {
//                 eat_character(lexer);
//                 return 10 + c - 'A';
//         } else if ((c >= '0') && (c <= '9')) {
//                 eat_character(lexer);
//                 return c - '0';
//         }
//
//         auto interp = lexer->interp;
//         lexer->parser->report_parse_error(
//             lexer, "A hex digit is required here.\n");
//
//         switch (type) {
//         case HEX_DIGIT_8BIT: {
//                 interp->report(
//                     REPORT_NOLABEL,
//                     lexer,
//                     NULL,
//                     "\\x must be followed by exactly two hex digits.");
//                 break;
//         }
//         case HEX_DIGIT_UNICODE_32: {
//                 interp->report(
//                     REPORT_NOLABEL,
//                     lexer,
//                     NULL,
//                     "\\u must be followed by exactly four hex digits.");
//                 break;
//         }
//         case HEX_DIGIT_UNICODE_64: {
//                 interp->report(
//                     REPORT_NOLABEL,
//                     lexer,
//                     NULL,
//                     "\\U must be followed by exactly eight hex digits.");
//                 break;
//         }
//         }
//
//         return -1;
// }
//
// int lexer_get_decimal_digit(struct lexer *lexer, enum Hex_Digit_Type type) {
//         assert(false && "unimplemented");
//
//         char c = peek_next_character(lexer);
//
//         if ((c >= '0') && (c <= '9')) {
//                 eat_character(lexer);
//                 return c - '0';
//         }
//
//         auto interp = lexer->interp;
//         lexer->parser->report_parse_error(
//             lexer, "A decimal digit is required here.\n");
//         interp->report(
//             REPORT_NOLABEL,
//             lexer,
//             NULL,
//             "\\U must be followed by exactly eight hex digits.");
//
//         return -1;
// }
//
// void lexer_set_end_of_token(struct lexer *lexer, struct token *token) {
//         token->l1 = current_line_number;
//         token->c1 = current_character_index;
// }

// struct token *lexer_peek_token(struct lexer *self, int lookahead_index) {
//         assert(lookahead < MAX_CONCURRENT_TOKENS);
//         assert(lookahead_index >= 0);
//
//         if (lookahead_index == 0)
//                 return lexer_peek_next_token();
//
//         while (self->num_incoming_tokens <= lookahead_index) {
//                 if (parser->reported_parse_error) {
//                         set_end_of_token(&eof_token);
//                         eof_token.l0 = eof_token.co;
//                 }
//         }
// }

// ----------------- TODO ---------------------- //
// ----------------------------------------------//

// —————————————————————————————————————————————————————————————————————————————————————
// HELPERS

static int buf_clear(char array[], size_t *array_length) {
        if (array == NULL)
                return -EBUSY; // see linux style guide
        for (int i = 0; i < *array_length; i++) {
                array[i] = '\0';
        }
        *array_length = 0;

        return 0; // success
}

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct token *lexer_tokenize(struct lexer *self, int *token_count) {
        *token_count = 0;
        size_t m_src_length = strlen(self->m_src);
        size_t size = m_src_length * sizeof(struct token);

        struct token *tokens = (struct token *)malloc(size);
        if (tokens == NULL) {
                perror("Failed to allocate memory for tokens while tokenizing\n");
                return NULL;
        }

        char   buf[m_src_length + 1]; // NOTE: initialize this?
        int    ofst = 0;
        size_t i = 0;

        while (peek_next_character(self) != '\0') {
                if (isalpha(peek_next_character(self))) {
                        buf[i++] = eat_character(self);
                        while (peek_next_character(self) != '\0' &&
                               isalnum(peek_next_character(self))) {
                                buf[i++] = eat_character(self);
                        }
                        buf[i] = '\0'; // Null-terminate the token buffer
                        if (strcmp(buf, "exit") == 0) {
                                tokens[(*token_count)++] = (struct token){.kind = TOKEN_EXIT};
                                buf_clear(buf, &i);
                        } else if (strcmp(buf, "let") == 0) {
                                tokens[(*token_count)++] = (struct token){.kind = TOKEN_LET};
                                buf_clear(buf, &i);
                        } else {
                                (tokens[(*token_count)++] =
                                     (struct token){.kind = TOKEN_IDENT, .value = strdup(buf)});
                                buf_clear(buf, &i);
                        }
                } else if (isdigit(peek_next_character(self))) {
                        buf[i++] = eat_character(self);
                        while (peek_next_character(self) != '\0' &&
                               isdigit(peek_next_character(self))) {
                                buf[i++] = eat_character(self);
                        }
                        buf[i] = '\0';
                        (tokens[(*token_count)++] =
                             (struct token){.kind = TOKEN_INT_LIT, .value = strdup(buf)});
                        buf_clear(buf, &i);
                } else if (peek_next_character(self) == '(') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_PAREN_OPEN});
                } else if (peek_next_character(self) == ')') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_PAREN_CLOSE});
                }
                /* TODO: comments */
                else if (peek_next_character(self) == '/' && peek_character(self, 1) == '/') {
                        assert(eat_character(self) == '/');
                        assert(eat_character(self) == '/');
                        while (isspace(peek_next_character(self))) {
                                eat_character(self);
                        }
                        while (peek_next_character(self) != '\n') {
                                buf[i++] = eat_character(self);
                        }
                        buf[i] = '\0';
                        self->total_code_lines_processed += 1;

                        (tokens[(*token_count)++] =
                             (struct token){.kind = TOKEN_COMMENT, .value = strdup(buf)});
                        buf_clear(buf, &i);
                }
                /* end of statement with line end `\n` (or semicolon in C) */
                else if (
                    peek_next_character(self) == '\n' && (*token_count > 0) &&
                    tokens[(*token_count - 1)].kind != TOKEN_COMMENT) { // == ';') {
                        eat_character(self);
                        tokens[(*token_count)++] = (struct token){.kind = TOKEN_ENDL};
                        self->code_lines_processed += 1;
                        self->total_code_lines_processed += 1;

                        while (peek_next_character(self) == '\n') {
                                assert(eat_character(self) == '\n');
                                self->total_code_lines_processed += 1;
                        }
                        continue;
                } else if (peek_next_character(self) == ':') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_COLON});
                } else if (peek_next_character(self) == '(') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_PAREN_OPEN});
                } else if (peek_next_character(self) == ')') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_PAREN_CLOSE});
                } else if (peek_next_character(self) == '?') {
                        eat_character(self);
                        // result = make_one_character_token('?');
                        // break;
                        (tokens[(*token_count)++] = (struct token){.kind = (enum Token_Kind)'?'});
                } else if (peek_next_character(self) == '{') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_CURLY_OPEN});
                } else if (peek_next_character(self) == '}') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_CURLY_CLOSE});
                } else if (peek_next_character(self) == '=') {
                        eat_character(self);
                        (tokens[(*token_count)++] = (struct token){.kind = TOKEN_ISEQUAL});
                } else if (isspace(peek_next_character(self))) {
                        eat_character(self);
                } else {
                        perror("You messed up while peeking!\n");
                        exit(EXIT_FAILURE);
                }
                if (i > m_src_length) {
                        perror("Temporary buffer stack overflowed while "
                               "tokenizing\n");
                        exit(EXIT_FAILURE);
                }
        }

        self->m_index = 0;

        return tokens;
}

#endif /* CF17C85F_D892_43F8_B47B_0BA265F170E6 */
