#include "sattest.h"

/**
 * Fonction pour allouer et intialiser une Formule.
 * Une formule aura une capacitÃ© de 10 clause et si elle devient pleine alors la capacitÃ© sera augmentÃ©e par incrÃ©ment de 10
 * @param nvars le nombre de variables utilisÃ©es dans la formule
 * @return la formule crÃ©Ã©e=
 */
Formula* formula_new(size_t nvars){
    //
    Formula* f=(Formula*)malloc(sizeof(Formula));
    if(f==NULL){perror("Echec d'allocation"); exit(EXIT_FAILURE);}
    //
    f->nvars = nvars;
    f->nclauses = 0;
    f->capacity = 10;
    //
    f->clauses  = (Clause**)malloc(f->capacity*sizeof(Clause*)); //alloue la memoire disponible pour les clauses
    if(f->clauses==NULL){perror("Echec d'allocation"); exit(EXIT_FAILURE);}
    //
    f->vars = (Variable*)malloc(sizeof(Variable) * (nvars + 1));
    for (size_t i = 1; i <= nvars; i++)
    {
        f->vars[i].assign = -1; //var[i]=-1 tous les elements sont non asssignés
        f->vars[i].pos_w = NULL;
        f->vars[i].neg_w = NULL;
    }
    //
    f->assigned = (var*)malloc((nvars + 1) * sizeof(var)); // a demander  pour lhistorique suis pas sur de combien je devrais allouer de meoir car ca peut depasser la nvars
    if(f->assigned==NULL){perror("Echec d'allocation"); exit(EXIT_FAILURE);}

    f->nassigned = 0; //nbre de variablesassigné s 0

    return f; //return f
}

/**
 * Fonction pour allouer et intialiser une Clause.
 * @param size le nombre de littÃ©raux dans la clause
 * @param lits le tableau des littÃ©raux qui seront copiÃ©s dans la clause
 * @return un pointeur var la clause crÃ©Ã©e
 */
Clause* clause_new(size_t size, lit lits[]){
    //
    Clause *c = (Clause*)malloc(sizeof(Clause)+size*sizeof(lit));
    if(c==NULL){
        perror("Erreur d'allocation");
        exit(EXIT_FAILURE);
    }
    //
    c->size = size;
    c->idx1 = 0;
    c->idx2 = (size > 1) ? 1 : 0;
    
    for (size_t i = 0; i < size; i++){
        c->lits[i]=lits[i];
    }
    return c;
}

void formula_add_clause(Formula* f, Clause* c){
    if((f->nclauses)+1==f->capacity){
        f->capacity += 10;
        Clause** c2 = realloc(f->clauses,f->capacity*sizeof(Clause*));
           if(c2==NULL){perror("Erreur d'allocation"); exit(EXIT_FAILURE);}
           f -> clauses = c2;
    }
    
    f->clauses[f->nclauses] = c;
    f->nclauses += 1;

    // Ajout logique watchnode ( Partie 3 intégrée ici )
    if(c->size == 0) return;
    
    watchnode_create_and_insert(f, c, c->idx1);
    if(c->size > 1){
        watchnode_create_and_insert(f, c, c->idx2);
    }
}

void formula_free(Formula*f){
    if(!f) return;
    for (size_t i = 0; i < f->nclauses;i++){
        free(f->clauses[i]);
    }
    free(f->clauses);
    
    // Libération des listes chaînées (Logique déplacée ici pour propreté)
    if(f->vars!=NULL){
        for (size_t i = 1; i <= f->nvars; i++){
            WatchNode* wl = f->vars[i].pos_w;
            while(wl!=NULL){
                WatchNode* temp = wl;
                wl = wl->next;
                free(temp);
            }
            wl = f->vars[i].neg_w;
            while(wl!=NULL){
                WatchNode* temp = wl;
                wl = wl->next;
                free(temp);
            }
        }
        free(f->vars);
    }
    
    if(f->assigned) free(f->assigned);
    free(f);
}