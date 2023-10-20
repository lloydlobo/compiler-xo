#ifndef DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00
#define DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00

/*
 * parsing should take less than 5% time of compilation
 *
 * TODO: implement recursive descent
 *  operator precedence binary operations like (1 + 2) * 3 - 4 * 5
 */

#include "lexer.h"
#include "lumlib.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* taken from jai livestream Discussion: Making Programming Languade Parsers,..

struct lexer;

struct tree_changer;

struct token;
struct lumina_interp;
struct atom;
struct ast_if;

const int PARENS_REQUIRED = 0x0;
const int PARENS_OPTIONAL = 0x1;
const int PARENS_NOT_PERMITTED = 0x2;

const int PRECEDENCE_CAST = 2; // @Volatile: J Blow comment: Needs to make sense
                               // with get_binary_operator_precedence

*/
struct neoparser {
        // ~parser(); // destructor
        //
        // void init(lumina_interp *interp);
        //
        // lumina_interp *interp;
        // pool pool;
        //
        // lexer lexer;
        // tree_changer tree_changer;
        // copier copier;
        //
        // int comma_list_depth = 0;
        // export_type current_export_type = SCOPE_EXPORT;
        // ast_expression  *current_scope_or_load_directive = NU...
        //
        // block_stack blocks;
        // bool reported_parse_error = false
        //
        // char *stack_base; // used for detecting stack overflow
        //
        // void init_stack_base();
        //
        // void parse_toplevel(ast_directive_load *load);
        //
        // bool eat_possible_directive(char *name);
        // bool peek_possible_directive(char *name);
        // ast_directive_modify *eat_possible_modify_directive(char *name);
        //
        // void put_inline_on_procedure_call(ast_procedure_call *call, bool
        //                                   doing_inline);
        // void handle_elsewhere_directive(ast_lambda *lambda, char*
        //                                 directive_name);
        //
        // ast_declaration *make_constant_declaration(utf8 *name, ast_expression
        //                                            *expression);
        // ast_declaration *make_constant_declaration(atom *atom, ast_expression
        //                                            *expression);
        // ast_declaration *make_constant_declaration(ast_ident *ident,
        //                                          ast_expression *expression);
        //
        // void build_arguments_and_returns_for_modify_directive(
        //              ast_directive_modify *modify, array <ast_declaration *>
        //              const &arguments, ast_expression  *owner);
        //
        // ast_declaration *make_parameter_declaration(ast_lambda *lambda,
        //              atom *name, ast_type_instantiation *inst);
        // ast_declaration *make_parameter_declaration(ast_lambda *lambda,
        //              atom *name, char *_type_name, bool is_pointer);
        //
        // ... more at 54:04 / 2:56:05
        //
        // // protected:
        //
        // ast_ident *parse_operator_ident();
        // void child_new_block(ast_block *block);
        // ...
        // // ...
        // me: note ast in ast.h, use node or ast prefix. node_declaration
};

enum NodeExprType {
        EXPR_INT_LIT,
        EXPR_IDENT,
        EXPR_INVALID = -1,
};

struct node_expr {
        enum NodeExprType type;
        union {
                struct token int_lit;
                struct token ident;
        } var;
};

enum NodeStmtType {
        STMT_EXIT,
        STMT_FUNCTION,
        STMT_LET_MUT,
        STMT_LET_IMUT,
        STMT_COMMENT,
        STMT_INVALID = -1,
};

struct node_stmt {
        enum NodeStmtType type;
        union {
                struct node_expr expr_function;
                struct node_expr expr_exit;
                struct {
                        struct token     ident;
                        struct node_expr expr;
                } expr_let;
                // TODO: add func here?
        } var;
};

struct node_prog {
        struct node_stmt *stmts;
        size_t            stmt_count;
};

/** Parser */
struct parser {
        struct token *m_tokens;
        size_t        m_token_count;
        size_t        m_index;
};

// —————————————————————————————————————————————————————————————————————————————————————
// INIT FREE

struct parser *parser_init(struct token *tokens, const size_t token_count) {
        struct parser *self = (struct parser *)malloc(sizeof(struct parser));
        if (self == NULL) {
                perror("Failed to allocate memory for Parser\n");
                return NULL;
        }
        if (tokens == NULL || token_count == 0) {
                perror("Empty tokens array passed while creating "
                       "parser\n");
                self->m_tokens = NULL;
        } else {
                self->m_tokens = tokens;
        }
        self->m_index = 0;
        self->m_token_count = token_count;

        return self;
}

void parser_free(struct parser *self) {
        if (self != NULL)
                free(self);
}

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static struct token *parser_peek_token(const struct parser *self, const int offset) {
        if (self->m_index + offset >= self->m_token_count)
                return NULL;
        return &self->m_tokens[self->m_index + offset];
}

static bool parser_peek_possible_token(struct parser *self, const int lookahead) {
        // can abstract this cmp everywhere into in bounds pvt method
        if (self->m_index + lookahead >= self->m_token_count)
                return false;
        if ((&self->m_tokens[self->m_index + lookahead] != NULL))
                return true;
        return true;
}

static bool parser_peek_possible_token_kind(
    struct parser *self, const int lookahead, enum Token_Kind kind) {
        if (self->m_index + lookahead >= self->m_token_count)
                return false;
        if ((&self->m_tokens[self->m_index + lookahead] != NULL) &&
            (self->m_tokens[self->m_index + lookahead].kind == kind))
                return true;
        return false;
}

static struct token *parser_eat_token(struct parser *self) {
        return &self->m_tokens[(self->m_index)++];
}

static err_t parser_eat_token_or_fail(
    struct parser *self, enum Token_Kind kind, const char *err_msg) {
        if (parser_peek_possible_token_kind(self, 0, kind)) {
                parser_eat_token(self);
                return ErrOk;
        }
        fprintf(stderr, "error: %s\n", err_msg);
        return ErrNotFound;
}

static void *parser_try_eat_possible_token(struct parser *self, enum Token_Kind kind) {
        if (!parser_peek_possible_token_kind(self, 0, kind))
                return NULL;
        return parser_eat_token(self);
}

// Taken from Jai, where directives start with `#`:  `#build :: { ... }`
bool parser_eat_possible_directive(struct parser *self, char *name) {
        // ... = lexer.peek_next_token();
        struct token *token = &self->m_tokens[self->m_index];
        if (token->kind != '#')
                return false;
        // ... = lexer.peek_token(1);
        struct token *next = &self->m_tokens[self->m_index + 1];
        // if ((token->type == TOKEN_IDENT)&&strcmp(next->name, name))
        if ((token->kind == TOKEN_IDENT) && strcmp(next->value, name)) {
                // if (next.ident_is_backticked) {
                //         report_parse_error( &lexer, "Can't use '`' in a directive.\n");
                // }
                // lexer.eat_token();
                // lexer.eat_token();
                return true;
        }
        return false;
}

// —————————————————————————————————————————————————————————————————————————————————————
// HELPERS

void report_parse_error(struct lexer *lexer, int line, const char *error_message) {
        fprintf(stderr, "Error at line %d: %s\n", line, error_message);
}

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct node_expr parser_parse_expr(struct parser *self) {
        if (parser_peek_token(self, 0) != NULL &&
            parser_peek_token(self, 0)->kind == TOKEN_INT_LIT) {
                return (struct node_expr){
                    .type = EXPR_INT_LIT, .var.int_lit = *parser_eat_token(self)};
        } else if (
            parser_peek_token(self, 0) != NULL && parser_peek_token(self, 0)->kind == TOKEN_IDENT) {
                return (struct node_expr){.type = EXPR_IDENT, .var.ident = *parser_eat_token(self)};
        } else {
                return (struct node_expr){.type = EXPR_INVALID};
        }
}

struct node_stmt parser_parse_stmt(struct parser *self) {
        struct node_stmt stmt;

        if (/* exit(0) */
            parser_peek_possible_token_kind(self, 0, TOKEN_EXIT) &&
            parser_peek_possible_token_kind(self, 1, TOKEN_PAREN_OPEN)) {
                assert(parser_eat_token(self)->kind == TOKEN_EXIT);
                assert(parser_eat_token(self)->kind == TOKEN_PAREN_OPEN);

                struct node_expr node_expr = parser_parse_expr(self);
                if (node_expr.type == EXPR_INVALID) {
                        fprintf(stderr, "error: Invalid expression\n");
                        goto fail;
                }
                stmt.type = STMT_EXIT;
                stmt.var.expr_exit = node_expr;

                if (parser_eat_token_or_fail(self, TOKEN_PAREN_CLOSE, "Expected `)`") != ErrOk)
                        goto fail;
                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;
        } else if (/* should DEPRECATE this */
                   parser_peek_possible_token_kind(self, 0, TOKEN_LET) &&
                   parser_peek_possible_token_kind(self, 1, TOKEN_IDENT) &&
                   parser_peek_possible_token_kind(self, 2, TOKEN_ISEQUAL)) {
                stmt.type = STMT_LET_MUT; // (let) `let x = 1;`

                stmt.var.expr_let.ident = *parser_eat_token(self);
                assert(parser_eat_token(self)->kind == TOKEN_ISEQUAL);

                struct node_expr nexpr = parser_parse_expr(self);
                if (nexpr.type == EXPR_INVALID) {
                        fprintf(stderr, "error: Invalid expression\n");
                        goto fail;
                }
                stmt.var.expr_let.expr = nexpr;

                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;
        } else if (/* `x := 1` mutable variable */
                   (parser_peek_possible_token_kind(self, 0, TOKEN_IDENT) &&
                    !parser_peek_possible_token_kind(self, 0, TOKEN_LET)) &&
                   (parser_peek_possible_token_kind(self, 1, TOKEN_COLON) &&
                    parser_peek_possible_token_kind(self, 2, TOKEN_ISEQUAL))) {
                stmt.type = STMT_LET_MUT;

                stmt.var.expr_let.ident = *parser_eat_token(self);
                assert(parser_eat_token(self)->kind == TOKEN_COLON);
                assert(parser_eat_token(self)->kind == TOKEN_ISEQUAL);

                struct node_expr nexpr = parser_parse_expr(self);
                if (nexpr.type == EXPR_INVALID) {
                        fprintf(stderr, "error: Invalid expression\n");
                        goto fail;
                }
                stmt.var.expr_let.expr = nexpr;

                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;
        } else if (/* `x :: 1` immutable constant */
                   (parser_peek_possible_token_kind(self, 0, TOKEN_IDENT) &&
                    !parser_peek_possible_token_kind(self, 0, TOKEN_LET)) &&
                   (parser_peek_possible_token_kind(self, 1, TOKEN_COLON) &&
                    parser_peek_possible_token_kind(self, 2, TOKEN_COLON)) &&
                   (!parser_peek_possible_token_kind(self, 3, TOKEN_PAREN_OPEN))) {
                stmt.type = STMT_LET_IMUT;

                stmt.var.expr_let.ident = *parser_eat_token(self);
                assert(parser_eat_token(self)->kind == TOKEN_COLON);
                assert(parser_eat_token(self)->kind == TOKEN_COLON);

                struct node_expr nexpr = parser_parse_expr(self);
                if (nexpr.type == EXPR_INVALID) {
                        fprintf(stderr, "error: Invalid expression\n");
                        goto fail;
                }
                stmt.var.expr_let.expr = nexpr;

                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;
        } else if (/* myfunction :: () { } */
                   (parser_peek_possible_token_kind(self, 0, TOKEN_IDENT) &&
                    !parser_peek_possible_token_kind(self, 0, TOKEN_LET)) &&
                   (parser_peek_possible_token_kind(self, 1, TOKEN_COLON) &&
                    parser_peek_possible_token_kind(self, 2, TOKEN_COLON)) &&
                   (parser_peek_possible_token_kind(self, 3, TOKEN_PAREN_OPEN))) {
                {
                        assert(parser_peek_token(self, 0)->kind == TOKEN_IDENT);
                        struct token *tok_func_ident = parser_eat_token(self);
                        stmt.var.expr_function.var.ident.value = (tok_func_ident->value);
                }
                assert(parser_eat_token(self)->kind == TOKEN_COLON);
                assert(parser_eat_token(self)->kind == TOKEN_COLON);
                assert(parser_eat_token(self)->kind == TOKEN_PAREN_OPEN);
                {
                        // ... recursion func parameter scope `(arg1:
                        // int, arg2: float)`
                }
                if (parser_eat_token_or_fail(self, TOKEN_PAREN_CLOSE, "Expected `)`") != ErrOk)
                        goto fail;
                if (parser_eat_token_or_fail(self, TOKEN_CURLY_OPEN, "Expected `{`") != ErrOk)
                        goto fail;
                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;
                {
                        // TODO: ... recursion block scope
                }
                if (parser_eat_token_or_fail(self, TOKEN_CURLY_CLOSE, "Expected `}`") != ErrOk)
                        goto fail;
                if (parser_eat_token_or_fail(self, TOKEN_ENDL, "Expected newline") != ErrOk)
                        goto fail;

                stmt.type = STMT_FUNCTION; // finally!!!
        } else if (parser_peek_possible_token_kind(self, 0, TOKEN_COMMENT)) {
                stmt.type = STMT_COMMENT;

                if (!parser_peek_possible_token(self, 0))
                        fprintf(stderr, "Empty Comment\n"); // info like warning

                parser_eat_token(self);
        } else {
                stmt.type = STMT_INVALID;

                perror("Syntax error\n");
        }

        return stmt;

fail:
        perror("error: Failed to parse statements\n");
        exit(EXIT_FAILURE);
}

struct node_prog *parser_parse_prog(struct parser *self) {
        struct node_prog *program = (struct node_prog *)malloc(sizeof(struct node_prog));
        if (program == NULL) {
                fprintf(stderr, "error: Failed to allocate memory for program\n");
                return NULL;
        }

        program->stmt_count = 0;
        program->stmts = (struct node_stmt *)malloc(10 * sizeof(struct node_stmt));

        if (program->stmts == NULL) {
                fprintf(stderr, "error: Failed to allocate memory for stmts\n");
                goto fail;
        }

        while (parser_peek_token(self, 0) != NULL) {
                struct node_stmt stmt = parser_parse_stmt(self);
                if (stmt.type == STMT_INVALID) {
                        fprintf(stderr, "error: Invalid statement\n");
                        goto fail;
                }
                program->stmt_count++;
                program->stmts = (struct node_stmt *)realloc(
                    program->stmts, program->stmt_count * sizeof(struct node_stmt));
                if (program->stmts == NULL) {
                        fprintf(stderr, "error: Failed to reallocate memory\n");
                        goto fail;
                }
                program->stmts[program->stmt_count - 1] = stmt;
        }

        return program;

fail:
        if (program->stmts != NULL)
                free(program->stmts);
        free(program);

        return NULL;
}

/* Print the parsed prog */
void p_node_prog_print(struct node_prog *self) {
        printf("Parsed prog:\n");
        for (size_t i = 0; i < self->stmt_count; i++) {
                struct node_stmt stmt = self->stmts[i];
                if (stmt.type == STMT_EXIT) {
                        printf(
                            "Exit Statement with code: %s\n", stmt.var.expr_exit.var.int_lit.value);
                } else if (stmt.type == STMT_LET_MUT) {
                        printf(
                            "Let Mutable Statement: %s := %s\n",
                            stmt.var.expr_let.ident.value,
                            stmt.var.expr_let.expr.var.int_lit.value);
                } else if (stmt.type == STMT_LET_IMUT) {
                        printf(
                            "Let Immutable Statement: %s :: %s\n",
                            stmt.var.expr_let.ident.value,
                            stmt.var.expr_let.expr.var.int_lit.value);
                } else if (stmt.type == STMT_FUNCTION) {
                        printf(
                            "Function Statement Block: %s :: (todo!) "
                            "-> { ... "
                            "}: %s\n",
                            stmt.var.expr_function.var.ident.value,
                            "");
                } else if (stmt.type == STMT_COMMENT) {
                        printf(
                            "Comment Statement with string value "
                            "todo!: %s\n",
                            "");
                } else {
                        fprintf(stderr, "error: Invalid Statement: %d\n", stmt.type);
                }
        }
}

// —————————————————————————————————————————————————————————————————————————————————————
// test

// static int test__parser() {
//         struct token tokens[] = {
//             {TOKEN_EXIT, NULL},
//             {TOKEN_PAREN_OPEN, NULL},
//             {TOKEN_INT_LIT, "42"},
//             {TOKEN_PAREN_CLOSE, NULL},
//             {TOKEN_SEMICOLON, NULL}};
//
//         size_t token_count = sizeof(tokens) / sizeof(tokens[0]);
//
//         struct parser *p = parser_init(tokens, token_count);
//
//         struct node_prog *program = parser_parse_prog(p);
//
//         if (program != NULL) {
//                 // Print the parsed program
//                 printf("Parsed Program:\n");
//                 for (size_t i = 0; i < program->stmt_count; i++) {
//                         struct node_stmt stmt = program->stmts[i];
//                         if (stmt.type == STMT_EXIT) {
//                                 printf(
//                                     "Exit Statement with code: %s\n",
//                                     stmt.var.expr_exit.var.int_lit.value);
//                         } else if (stmt.type == STMT_LET_MUT) {
//                                 printf(
//                                     "Let Statement: %s = %s\n",
//                                     stmt.var.expr_let.ident.value,
//                                     stmt.var.expr_let.expr.var.int_lit.value);
//                         }
//                 }
//                 free(program->stmts);
//                 free(program);
//         }
//         parser_free(p);
//
//         return 0;
// }

#endif /* DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00 */
