#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../include/parser.h"
#include "../include/scanner.h"
#include "../include/types.h"

// TODO: need a good way to free tree nodes in case of error
//   - Could create our own allocator with a global array...
//   - but then limited space: need to set array length

typedef struct {
    Token current;
    Token next;
    bool hadError;
    char pad[7];
} Parser;

static Parser parser;
static int varId = 0;
static VarList* varList;

static Term* term(void);
static Term* application(void);
static Term* atom(void);

static void errorAt(Token* token, const char* message) {
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}

static void advance() {
    for (;;) {
        parser.current = parser.next;
        parser.next = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent((const char*)parser.current.start);
    }
}

static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool match(TokenType type, const char* message) {
    if (parser.current.type != type) {
        errorAtCurrent(message);
        return false;
    } else {
        return true;
    }
}

static Term* atom() {
    if (parser.current.type == TOKEN_LEFT_PAREN) {
        advance();
        Term* t = term();
        consume(TOKEN_RIGHT_PAREN, "Expected right parenthese.");
        return t;
    }

    if (parser.current.type == TOKEN_VAR) {
        Term* t = allocTerm(termHeap);
        t->type = TERM_VAR;

        char varName = (char)parser.current.start[0];

        advance();

        bool foundVar = false;
        for (int i = 0; i < varList->size; i++) {
            Var v = varList->vars[i];

            if (v.name == varName) {
                t->term.var = v;
                foundVar = true;
            }
        }

        // FREE VARIABLE
        if (!foundVar) {
            Var var = { .id = varId++, .name = varName };
            t->term.var = var;
        }

        return t;
    }

    return NULL;
}

static Term* application() {
    Term* left = atom();

    if (left == NULL) {
        return NULL;
    }

    while (true) {
        Term* right = atom();

        if (right == NULL) {
            return left;
        }

        Term* p = allocTerm(termHeap);
        p->type = TERM_APP;
        p->term.app.first = left;
        p->term.app.second = right;

        left = p;
    }
}

static Term* term() {
    if (parser.current.type == TOKEN_LAMBDA) {
        advance();

        // Match variable
        if (!match(TOKEN_VAR, "Expected variable.")) {
            return NULL;
        }

        Term* first = NULL;

        Term* cur = NULL;
        int count = 0;
        while (parser.current.type == TOKEN_VAR) {
            Term* t = allocTerm(termHeap);
            t->type = TERM_LAMBDA;

            // Create variable
            Var var = { .id = varId++, .name = (char)parser.current.start[0] };
            t->term.lambda.var = var;

            // Add variable to variable list
            pushVarList(varList, var);

            advance();

            if (cur == NULL) {
                first = t;
            } else {
                cur->term.lambda.body = t;
            }
            cur = t;
            count++;
        }

        // Match period
        if (!match(TOKEN_PERIOD, "Expected period.")) {
            return NULL;
        }
        advance();

        // Get body
        Term* body = term();

        if (body == NULL) {
            return NULL;
        }

        cur->term.lambda.body = body;

        for (int i = 0; i < count; i++)
            popVarList(varList);

        return first;
    } else {
        return application();
    }
}

Term* interpret(const char* source) {
    varList = initVarList(100);

    initScanner(source);
    advance();
    // To update current
    advance();
    Term* t = term();
    consume(TOKEN_EOF, "Expect end of expression.");

    // Should not be any variables still on the stack...
    assert(varList->size == 0);

    freeVarList(varList);

    return t;
}
