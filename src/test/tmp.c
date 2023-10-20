struct node_stmt p_parse_stmt(struct parser *self)
{
    struct node_stmt stmt;

    // Determine the type of statement based on the first token
    switch (p_peek(self, 0)->type) {
    case TEXIT:
        if (!(p_peek(self, 1) != NULL && p_peek(self, 1)->type == TPAREN_OPEN))
            break;

        p_consume(self);
        p_consume(self);

        struct node_expr node_expr = p_parse_expr(self);
        switch (node_expr.type) {
        case -1:
            fprintf(stderr, "[%d]: error: Invalid expression\n", __LINE__);
            goto fail;
        default:
            stmt.type = STMT_EXIT;
            stmt.var.exit_expr = node_expr;
            break;
        }

        // Handle TPAREN_CLOSE and TSEMICOLON using switch statements
        switch (p_peek(self, 0)->type) {
        case TPAREN_CLOSE:
            p_consume(self);
            break;
        case TSEMICOLON:
            p_consume(self);
            break;
        default:
            fprintf(stderr, "[ERROR] Expected `)` or `;`\n");
            goto fail;
            break;
        }
        break;

    case TLET:
        if (!(p_peek(self, 1) != NULL && p_peek(self, 1)->type == TIDENT
              && p_peek(self, 2) != NULL && p_peek(self, 2)->type == TEQUAL))
            break;

        p_consume(self);

        stmt.type = STMT_LET;
        stmt.var.let_expr.ident = *p_consume(self);

        p_consume(self);

        struct node_expr expr = p_parse_expr(self);
        switch (expr.type) {
        case -1:
            fprintf(stderr, "[%d]: error: Invalid expression\n", __LINE__);
            goto fail;
        default:
            stmt.var.let_expr.expr = expr;
            break;
        }

        // Handle TSEMICOLON using a switch statement
        switch (p_peek(self, 0)->type) {
        case TSEMICOLON:
            p_consume(self);
            break;
        default:
            fprintf(stderr, "[ERROR] Expected `;`\n");
            goto fail;
            break;
        }
        break;

    default:
        stmt.type = -1; // Invalid statement
        break;
    }

    return stmt;

fail:
    perror("[ERROR] Parser stmt\n");
    exit(EXIT_FAILURE);
}

