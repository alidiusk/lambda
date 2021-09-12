#ifndef TYPES_H
#define TYPES_H

typedef struct Term Term;

typedef struct {
    int id;
    char name;
    char pad[3];
} Var;

typedef struct {
    Var var;
    Term* body;
} Lambda;

typedef struct {
    Term* first;
    Term* second;
} Application;

typedef union {
    Var var;
    Lambda lambda;
    Application app;
} TermU;

enum TermType {
    TERM_VAR,
    TERM_LAMBDA,
    TERM_APP,
};

// One of (var, lambda, application).
// Other two should be NULL
struct Term {
    enum TermType type;
    char pad[4];
    TermU term;
};

typedef struct {
    int size;
    int cap;
    Var* vars;
} VarList;

typedef struct {
    int size;
    int cap;
    Term* heap;
} TermHeap;

TermHeap* initTermHeap(int cap);
void freeTermHeap(TermHeap* th);
void reallocTermHeap(TermHeap* th);
Term* allocTerm(TermHeap* th);

VarList* initVarList(int cap);
void freeVarList(VarList* vl);
void reallocVarList(VarList* vl);
void pushVarList(VarList* vl, Var v);
Var popVarList(VarList* vl);

extern TermHeap* termHeap;

#endif
