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

static Term* expression() {
    Term* t = allocTerm(termHeap);

    switch (parser.current.type) {
        // deal with parens
        case TOKEN_LEFT_PAREN: {
            advance();
            t = expression();
            consume(TOKEN_RIGHT_PAREN, "Expect right parenthese.");
            break;
        }
        case TOKEN_LAMBDA: {
            t->type = TERM_LAMBDA;
            advance();

            // Match variable
            if (!match(TOKEN_VAR, "Expected variable.")) {
                return NULL;
            }

            // Create variable
            Var var = { .id = varId++, .name = (char)parser.current.start[0] };
            t->term.lambda.var = var;

            // Match period
            advance();
            if (!match(TOKEN_PERIOD, "Expected period.")) {
                return NULL;
            }
            advance();

            // Add variable to variable list
            pushVarList(varList, var);

            // Get body
            Term* body = expression();

            popVarList(varList);

            if (body == NULL) {
                return NULL;
            }

            t->term.lambda.body = body;

            break;
        }
        case TOKEN_VAR: {
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

            break;
        }
        case TOKEN_PERIOD:
            errorAtCurrent("Unexpected period.");
            return NULL;
        case TOKEN_RIGHT_PAREN:
            errorAtCurrent("Unexpected right paranthese.");
            return NULL;
        case TOKEN_EOF:
            errorAtCurrent("Unexpected end of expression.");
            return NULL;
        case TOKEN_ERROR:
            errorAtCurrent("Encountered token error.");
            return NULL;
    }

    // TODO: make left-associative

    // In case there is an application.
    switch (parser.current.type) {
        case TOKEN_LEFT_PAREN:
        case TOKEN_LAMBDA:
        case TOKEN_VAR: {
            Term* s = expression();

            if (s == NULL) {
                return NULL;
            }

            Term* p = allocTerm(termHeap);
            p->type = TERM_APP;
            p->term.app.first = t;
            p->term.app.second = s;

            return p;
        }
        case TOKEN_RIGHT_PAREN:
        case TOKEN_PERIOD:
        case TOKEN_EOF:
        case TOKEN_ERROR:
            return t;
    }
}

Term* interpret(const char* source) {
    varList = initVarList(100);

    initScanner(source);
    advance();
    // To update current
    advance();
    Term* t = expression();
    consume(TOKEN_EOF, "Expect end of expression.");

    // Should not be any variables still on the stack...
    assert(varList->size == 0);

    freeVarList(varList);

    return t;
}
