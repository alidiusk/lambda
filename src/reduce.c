#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/types.h"
#include "../include/reduce.h"

static bool varEquals(Var a, Var b) {
    return a.id == b.id && a.name == b.name;
}

static void substituteVar(Term* root, Var v, Term* t) {
    switch (root->type) {
        case TERM_APP:
            substituteVar(root->term.app.first, v, t);
            substituteVar(root->term.app.second, v, t);
            break;
        case TERM_LAMBDA:
            substituteVar(root->term.lambda.body, v, t);
            break;
        case TERM_VAR:
            if (varEquals(root->term.var, v)) {
                *root = *t;
            }
            break;
    }
}

static Term* lambdaApply(Term* lambda, Term* arg) {
    if (lambda->type != TERM_LAMBDA) {
        return NULL;
    }

    Var lambdaV = lambda->term.lambda.var;

    substituteVar(lambda->term.lambda.body, lambdaV, arg);

    return lambda->term.lambda.body;
}

Term* reduce(Term* term) {
    switch (term->type) {
        case TERM_APP: {
            Term* first = term->term.app.first;
            Term* second = term->term.app.second;

            // Beta reduction
            Term* applied = 0;
            if (first->type == TERM_LAMBDA) {
                applied = lambdaApply(first, second);
            }

            return applied;
        }
        case TERM_LAMBDA:
            term->term.lambda.body = reduce(term->term.lambda.body);
            return term;
        case TERM_VAR:
            return term;
    }
}
