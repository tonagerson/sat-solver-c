#ifndef SAT_H
#define SAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int32_t lit;
typedef int16_t var;

// Pré-déclaration
typedef struct Clause Clause;
typedef struct WatchNode WatchNode;

// Structures avec TES noms
struct WatchNode {
    Clause* clause;
    WatchNode* next;
    size_t lit_idx; 
};

typedef struct Variable {
    int8_t assign; // -1 unassigned, 0 false, 1 true
    struct WatchNode* pos_w; // Tête de liste des clauses surveillant la forme POSITIVE 
    struct WatchNode* neg_w; // Tête de liste des clauses surveillant la forme NEGATIVE
} Variable;

struct Clause {
    size_t size;
    size_t idx1; // Index (dans lits[]) du 1er littéral surveillé
    size_t idx2; // 2ee surveillé
    lit lits[];
};

typedef struct Formula {
    size_t nvars;
    size_t nclauses;
    size_t capacity;

    Clause** clauses;
    Variable* vars;

    var* assigned;
    size_t nassigned;
} Formula;

// Prototypes
Formula* formula_new(size_t nvars);
void formula_free(Formula* f);
Clause* clause_new(size_t size, lit lits[]);
void formula_add_clause(Formula* f, Clause* c);
bool assign(Formula* f, uint16_t var, bool value);
int8_t lit_eval(Formula* f, lit l);
bool formula_eval(Formula* f);
void watchnode_create_and_insert(Formula* f, Clause* c, size_t watch_idx);
bool update_watched(Formula* f, var v);
bool propagate(Formula *f, size_t from);
void backtrack(Formula* f, var v);
bool solve(Formula* f);


Formula* read_cnf(const char* fname);
#endif









