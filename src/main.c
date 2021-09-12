#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/scanner.h"
#include "../include/types.h"
#include "../include/parser.h"
#include "../include/print.h"
#include "../include/reduce.h"

char* readFile(const char* path);
void repl(void);
void runFile(const char* path);

char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = (size_t)ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void repl() {
    char line[1024];

    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        Term* t = interpret(line);
        if (t == NULL) {
            continue;
        }

        printTerm(t);
        printf("\n");

        t = reduce(t);
        if (t == NULL) {
            continue;
        }

        printTerm(t);
        printf("\n");
    }
}

void runFile(const char* path) {
    char* source = readFile(path);
    Term* t = interpret(source);

    free(source);

    if (t == NULL) {
        return;
    }

    t = reduce(t);

    if (t == NULL) {
        return;
    }

    printTerm(t);
    printf("\n");

    /* if (result == INTERPRET_COMPILE_ERROR) exit(65); */
    /* if (result == INTERPRET_RUNTIME_ERROR) exit(70); */
}

int main(int argc, const char* argv[]) {
    termHeap = initTermHeap(200);

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: lambda [path]\n");
        exit(64);
    }

    freeTermHeap(termHeap);

    return 0;
}
