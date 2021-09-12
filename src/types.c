#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/types.h"

TermHeap* termHeap;

TermHeap* initTermHeap(int cap) {
    TermHeap* th = (TermHeap*)malloc(sizeof(TermHeap));

    Term* heap = (Term*)malloc((unsigned long)cap * sizeof(Term));

    th->size = 0;
    th->cap = cap;
    th->heap = heap;

    return th;
}

void reallocTermHeap(TermHeap* th) {
    Term* heap = (Term*)malloc((unsigned long)th->cap * 2 * sizeof(Term));

    memcpy(heap, th->heap, (unsigned long)th->size * sizeof(Term));

    free(th->heap);

    th->heap = heap;
}

Term* allocTerm(TermHeap* th) {
    if (th->size >= th->cap) {
        reallocTermHeap(th);
    }

    th->size++;

    return &th->heap[th->size - 1];
}

void freeTermHeap(TermHeap* th) {
    free(th->heap);
    free(th);
}

VarList* initVarList(int cap) {
    VarList* vl = (VarList*)malloc(sizeof(VarList));

    Var* vars = (Var*)malloc((unsigned long)cap * sizeof(Var));

    vl->size = 0;
    vl->cap = cap;
    vl->vars = vars;

    return vl;
}

void reallocVarList(VarList* vl) {
    Var* vars = (Var*)malloc((unsigned long)vl->cap * 2 * sizeof(Var));

    memcpy(vars, vl->vars, (unsigned long)vl->size * sizeof(Var));

    free(vl->vars);

    vl->vars = vars;
}

void freeVarList(VarList* vl) {
    free(vl->vars);
    free(vl);
}

void pushVarList(VarList* vl, Var v) {
    if (vl->size >= vl->cap) {
        reallocVarList(vl);
    }

    vl->vars[vl->size] = v;

    vl->size++;
}

Var popVarList(VarList* vl) {
    if (vl->size != 0) {
        Var v = vl->vars[vl->size - 1];

        vl->vars[vl->size - 1] = (Var){0};
        vl->size--;

        return v;
    }

    return (Var){0};
}
