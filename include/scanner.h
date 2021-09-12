#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LAMBDA, TOKEN_PERIOD,
    // Single-character variable
    TOKEN_VAR,
    TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char pad[4];
    const unsigned char* start;
    int length;
    int line;
} Token;

void initScanner(const char* source);
Token scanToken(void);

#endif
