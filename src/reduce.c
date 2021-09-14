#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/types.h"
#include "../include/reduce.h"
#include "../include/print.h"

typedef struct {
    bool step;
    char pad[7];
    Term* term;
} ReductionResult;

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
    Var lambdaV = lambda->term.lambda.var;

    substituteVar(lambda->term.lambda.body, lambdaV, arg);

    return lambda->term.lambda.body;
}

// `cbn` and `reduce` adapted from: http://www.itu.dk/people/sestoft/papers/sestoft-lamreduce.pdf

static Term* cbn(Term* term) {
    switch (term->type) {
        case TERM_VAR:
            return term;
        case TERM_LAMBDA:
            return term;
        case TERM_APP: {
            Term* t = cbn(term->term.app.first);
            if (t->type == TERM_LAMBDA) {
                return cbn(lambdaApply(t, term->term.app.second));
            } else {
                term->term.app.first = t;
                return term;
            }
        }
    }
}

Term* reduce(Term* term) {
    switch (term->type) {
        case TERM_VAR:
            return term;
        case TERM_LAMBDA:
            term->term.lambda.body = reduce(term->term.lambda.body);
            return term;
        case TERM_APP: {
            Term* t = cbn(term->term.app.first);
            if (t->type == TERM_LAMBDA) {
                return reduce(lambdaApply(t, term->term.app.second));
            } else {
                t = reduce(t);
                Term* r = reduce(term->term.app.second);
                term->term.app.first = t;
                term->term.app.second = r;
                return term;
            }
        }
    }
}

/* ReductionResult reduceInner(Term* term) { */
/*     switch (term->type) { */
/*         case TERM_APP: { */
/*             Term* first = term->term.app.first; */
/*             Term* second = term->term.app.second; */
/*  */
/*             // Beta reduction */
/*             if (first->type == TERM_LAMBDA) { */
/*                 ReductionResult r = lambdaApply(first, second); */
/*                 return r; */
/*             } else { */
/*                 ReductionResult r = {0}; */
/*  */
/*                 do { */
/*                     r = reduceInner(first); */
/*                     term->term.app.first = r.term; */
/*                 } while (r.step); */
/*  */
/*                 do { */
/*                     r = reduceInner(second); */
/*                     term->term.app.second = r.term; */
/*                 } while (r.step); */
/*  */
/*                 r.term = term; */
/*                 return r; */
/*             } */
/*         } */
/*         case TERM_LAMBDA: { */
/*             ReductionResult r = {0}; */
/*  */
/*             do { */
/*                 r = reduceInner(term->term.lambda.body); */
/*                 term->term.lambda.body = r.term; */
/*             } while (r.step); */
/*  */
/*             r.term = term; */
/*             return r; */
/*         } */
/*         case TERM_VAR: { */
/*             ReductionResult r = { .step = false, .term = term }; */
/*             return r; */
/*         } */
/*     } */
/* } */
/*  */
/* Term* reduce(Term* term) { */
/*     ReductionResult r = reduceInner(term); */
/*  */
/*     while (r.step) { */
/*         r = reduceInner(r.term); */
/*     } */
/*  */
/*     return r.term; */
/* } */
