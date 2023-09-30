#ifndef DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00
#define DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00

#include "tokenizer.h"

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
    } var; // variant // FIXME: this seems anonymous (typedef vs global
           // forwarding?)
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
// TODO:    static inline Token try_consume(TokenType type, const std::string&
// err_msg);
// TODO:    static inline std::optional<Token> try_consume(TokenType type);

struct parser *parser_init(struct token *tokens, const size_t token_count)
{
    struct parser *self
        = (struct parser *)(malloc(token_count * sizeof(struct parser)));
    if (self == NULL) {
        perror("Failed to allocate memory for Parser\n");
        return NULL; // FIXME: panic if malloc returns NULL, do not return
                     // NULL!!
    }
    if (tokens == NULL || token_count == 0) {
        perror("Empty tokens array passed while creating parser\n");
        self->m_tokens = NULL;
    }
    else {
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
    struct node_prog
        *prog; // TODO: malloc in prog and free memory after calling this func
    {
        int OFFSET = 0;
        while (parser_peek(self, OFFSET) != NULL) {
            struct node_stmt *stmt = parser_parse_stmt(self);
            if (stmt != NULL) {
                {
                    // TODO
                }
                return NULL;
            }
            else {
                fprintf(
                    stderr,
                    "In function `parser_parse_prog`#while#if 1(%d):\n",
                    __LINE__);
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

#endif /* DC6E0D4D_9AB8_4772_A498_BCCAE04F1B00 */
