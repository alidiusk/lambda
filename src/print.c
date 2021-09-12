#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/types.h"
#include "../include/print.h"

void printTerm(Term* term) {
    switch (term->type) {
        case TERM_APP:
            printf("(");
            printTerm(term->term.app.first);
            printf(")(");
            printTerm(term->term.app.second);
            printf(")");
            break;
        case TERM_LAMBDA:
            printf("\u03bb%c.", term->term.lambda.var.name);
            printTerm(term->term.lambda.body);
            break;
        case TERM_VAR:
            printf("%c", term->term.var.name);
            break;
    }
}
