#include "sattest.h"

bool propagate(Formula *f, size_t from) {
    // On parcourt la pile des assignations à partir de l'indice 'from'
    // ATTENTION : f->nassigned peut augmenter PENDANT la boucle 
    // (c'est l'effet "cascade" de la propagation unitaire)
    for (size_t i = from; i < f->nassigned; i++) {
        
        // 1. On récupère la variable qui a été assignée
        var v = f->assigned[i];

        // 2. On met à jour les surveillants pour cette variable
        // Si update_watched retourne false, c'est qu'un conflit a été trouvé
        if (!update_watched(f, v)) {
            return false; // Conflit détecté, on arrête tout
        }
    }

    // Si on arrive ici, c'est que toutes les propagations sont finies sans conflit
    return true; 
}


void backtrack(Formula* f, var v) {
    if (!f) return;

    // Chercher la position de v dans la pile
    size_t idx = f->nassigned;
    for (size_t i = 0; i < f->nassigned; i++) {
        if (f->assigned[i] == v) {
            idx = i;
            break;
        }
    }

    if (idx == f->nassigned) {
        // v n’est pas dans la pile -> rien à faire
        return;
    }

    // Annuler toutes les assignations à partir de idx
    for (size_t i = idx; i < f->nassigned; i++) {
        var var_to_reset = f->assigned[i];
        f->vars[var_to_reset].assign = -1;
    }

    // Réduire la taille de la pile
    f->nassigned = idx;
}