#include "sattest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Formula* read_cnf(const char* fname) {
    FILE* file = fopen(fname, "r");
    if (!file) {
        perror("Erreur ouverture fichier CNF");
        return NULL;
    }

    char line[1024];
    size_t nvars = 0, nclauses = 0;
    Formula* f = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c' || line[0] == '\n' || line[0] == '\r') continue;
        if (line[0] == 'p') {
            char format[32];
            if (sscanf(line, "p %31s %zu %zu", format, &nvars, &nclauses) >= 3) {
                f = formula_new(nvars);
                break;
            }
        }
    }

    if (!f) {
        fclose(file);
        return NULL;
    }

    lit clause_lits[2048];
    size_t count = 0;
    int val = 0;

    while (fscanf(file, "%d", &val) == 1) {
        if (val != 0) {
            clause_lits[count++] = (lit)val;
        } else {
            if (count > 0) {
                Clause* c = clause_new(count, clause_lits);
                formula_add_clause(f, c);
                count = 0;
            }
        }
    }

    fclose(file);
    return f;
}
