#include "sattest.h"
#include <time.h>

int main(int argc, char* argv[])
{
    // Vérification des arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.cnf\n", argv[0]);
        return 1;
    }

    printf("Lecture du fichier : %s\n", argv[1]);
    
    // 1. Lecture du fichier CNF (utilise parser.c)
    Formula* f = read_cnf(argv[1]);
    
    printf("Formule chargee : %zu variables, %zu clauses.\n", f->nvars, f->nclauses);
    
    // 2. Mesure du temps
    clock_t start = clock();

    // 3. Résolution (utilise ton algo DPLL optimisé)
    bool result = solve(f);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // 4. Affichage du résultat
    if (result) {
        printf("RESULTAT: SATISFIABLE\n");
        /* Décommente si tu veux voir les valeurs
        for(int i=1; i<=f->nvars; i++) {
            printf("x%d=%d ", i, f->vars[i].assign);
        }
        printf("\n");
        */
    } else {
        printf("RESULTAT: INSATISFIABLE\n");
    }

    printf("Temps de calcul : %f secondes\n", time_spent);

    // 5. Nettoyage
    formula_free(f);
    return 0;
}