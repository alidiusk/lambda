#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../include/scanner.h"

typedef struct {
    const unsigned char* start;
    const unsigned char* current;
    char pad[4];
    int line;
} Scanner;

static Scanner scanner;

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = (const unsigned char*)message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static unsigned char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static unsigned char peek() {
    return *scanner.current;
}

static void skipWhitespace() {
    for (;;) {
        unsigned char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            default:
                return;
        }
    }
}

static bool isAlpha(unsigned char c) {
    return (c >= 'a' && c <= 'z');
}

void initScanner(const char* source) {
    scanner.start = (const unsigned char*)source;
    scanner.current = (const unsigned char*)source;
    scanner.line = 1;
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    unsigned char c = advance();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '.': return makeToken(TOKEN_PERIOD);
        case 0xCE:
            if (peek() == 0xBB) {
                advance();
                return makeToken(TOKEN_LAMBDA);
            }
            break;
    }

    if (isAlpha(c)) return makeToken(TOKEN_VAR);

    printf("Err token: %d\n", c);
    return errorToken("Unexpected character.");
}
