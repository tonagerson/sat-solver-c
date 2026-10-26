#include "sattest.h"

bool solve(Formula* f) {
    // Pile des variables de décision (différente de f->assigned)
    var* decisions = (var*)malloc((f->nvars + 1) * sizeof(var));
    size_t ndecisions = 0;

    // Propagation initiale
    if (!propagate(f, 0)) {
        free(decisions);
        return false;
    }

    while (true) { // tant que pas encore satisfait
        
        // Si tout est assigné
        if (f->nassigned == f->nvars) {
            free(decisions);
            return true;
        }

        // Choisir une variable non assignée
        var next_var = 0;
        for (size_t i = 1; i <= f->nvars; i++) {
            if (f->vars[i].assign == -1) {
                next_var = i;
                break;
            }
        }

        // Empiler (Décision) et assigner à vrai
        decisions[ndecisions++] = next_var;
        assign(f, next_var, true);

        // Boucle de Conflit / Propagation
        while(true) {
            if (propagate(f, f->nassigned - 1)) {
                 break; // Propagation OK, on retourne choisir une variable
            }

            // Conflit → backtrack
            if (ndecisions == 0) {
                 free(decisions);
                 return false; // plus de variable libre et conflit partout
            }

            var last_decision = decisions[ndecisions - 1];
            int8_t old_val = f->vars[last_decision].assign;

            backtrack(f, last_decision);
            ndecisions--; // On dépile

            if (old_val == 1) {
                // On essaie faux
                assign(f, last_decision, false);
                // On reste dans le while pour propager ce faux
            } else {
                // On a tout essayé pour cette variable, on laisse le while remonter au parent
            }
        }
    }
}