#include "sattest.h"

void watchnode_create_and_insert(Formula* f, Clause* c, size_t watch_idx){
     WatchNode *w = (WatchNode*)malloc(sizeof(WatchNode));
     if(w==NULL){
         perror("yo alloue bien");
         exit(EXIT_FAILURE);
     }
     //d'abord je vois le litteral surveille
     //je cherche sa variable
     //je remonte la formule 
     //si litteral postif je le mets dans watch pos du tableau de variable

     w->clause = c;
     w->lit_idx = watch_idx;
     
     lit val=c->lits[watch_idx];
     var n = abs(val);
     if(val>0){
         w->next = f->vars[n].pos_w;
         f->vars[n].pos_w = w;
     }else{
          w->next = f->vars[n].neg_w;
          f->vars[n].neg_w = w ;
     }
}

bool update_watched(Formula *f, var v) {
    // 1. Validation et récupération de l'assignation
    if (!f) return true;
    int8_t aval = f->vars[v].assign;
    
    // Si v=1, on surveille les clauses où v apparait en négatif (car elles deviennent fausses)
    // Si v=0, on surveille les clauses où v apparait en positif
    WatchNode **head_ptr = (aval == 1) ? &f->vars[v].neg_w : &f->vars[v].pos_w; 

    WatchNode *prev = NULL;
    WatchNode *curr = *head_ptr;

    while (curr != NULL) {
        // SAUVEGARDE CRITIQUE : On sauve le suivant car curr peut être déplacé ou supprimé
        WatchNode *next_node = curr->next;

        Clause *c = curr->clause;
        
        // idx_watched : l'index dans la clause du littéral qu'on est en train de visiter (celui qui est Faux)
        size_t idx_watched = curr->lit_idx;
        
        // idx_other : l'index de l'autre littéral surveillé
        size_t idx_other = (idx_watched == c->idx1) ? c->idx2 : c->idx1;
        lit lit_other = c->lits[idx_other];

        // --- INVARIANT : Si l'autre surveillant est Vrai, la clause est satisfaite ---
        // On passe simplement au suivant sans rien changer
        if (lit_eval(f, lit_other) == 1) { 
            prev = curr;
            curr = next_node;
            continue;
        }

        // --- RECHERCHE DE REMPLAÇANT ---
        size_t replacement = (size_t)-1;
        for (size_t i = 0; i < c->size; i++) {
            if (i == c->idx1 || i == c->idx2) continue; // On ne prend pas les surveillants actuels
            
            // On cherche un littéral qui n'est PAS FAUX (donc Vrai ou Non Assigné)
            if (lit_eval(f, c->lits[i]) != 0) {
                replacement = i;
                break;
            }
        }

        // --- CAS 1 : REMPLAÇANT TROUVÉ ---
        if (replacement != (size_t)-1) {
            // A. Mise à jour de la CLAUSE (C'est ce qui manquait !)
            if (c->idx1 == idx_watched) c->idx1 = replacement;
            else c->idx2 = replacement;

            // B. Mise à jour du WATCHNODE
            curr->lit_idx = replacement;

            // C. DÉPLACEMENT DU WATCHNODE (Plomberie des listes)
            // 1. On l'enlève de la liste courante
            if (prev == NULL) {
                *head_ptr = next_node; // C'était la tête
            } else {
                prev->next = next_node; // On ponte par-dessus
            }

            // 2. On l'insère dans la nouvelle liste
            lit new_lit = c->lits[replacement];
            var new_var = abs(new_lit);
            
            WatchNode** dest_list = (new_lit > 0) ? &f->vars[new_var].pos_w : &f->vars[new_var].neg_w;
            
            curr->next = *dest_list;
            *dest_list = curr;

            // D. On continue la boucle
            curr = next_node; 
            continue;
        }

        // --- CAS 2 : PAS DE REMPLAÇANT (Unitaire ou Conflit) ---
        int8_t res_other = lit_eval(f, lit_other);

        if (res_other == 0) {
            // CONFLIT : L'autre surveillant est aussi FAUX
            return false;
        } 
        else { // res_other == -1 (Non assigné)
            // UNITAIRE : La clause force l'assignation
            var var_to_assign = abs(lit_other);
            bool val_to_assign = (lit_other > 0) ? true : false;
            
            // On tente l'assignation
            if (!assign(f, var_to_assign, val_to_assign)) {
                return false;
            }
        }
        // On avance
        prev = curr;
        curr = next_node;
    }
    return true;
}