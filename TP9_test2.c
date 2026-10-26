#include "sattest.h"

bool assign(Formula* f, uint16_t var, bool value){
    if(f->vars[var].assign!=-1 && f->vars[var].assign!= (int8_t)value){
        return false;
    } 
    if (f->vars[var].assign == (int8_t)value) {
        return true;
    }
    
    f->vars[var].assign = (int8_t)value;
    f->assigned[f->nassigned] = var;
    f->nassigned = f->nassigned + 1;
    return true;
}

int8_t lit_eval (Formula*f , lit l){
    int8_t n = f->vars[abs(l)].assign;
    if(n==-1){
        return -1;
    } else {
        return (l>0) ? n : (n==0 ? 1 : 0);
    }
} 

int8_t clause_eval(Formula*f, Clause* c){
    int8_t non = 0; //ou int8_t je sias pas  
    for (size_t i = 0; i < c->size;i++){
        if(lit_eval(f,c->lits[i])==1){ // j'aurai pu mettre c->its [i] dans une variable
            return 1;
        }
        if(lit_eval(f,c->lits[i])==-1){
             non = -1;
        }
    }
    return (non == -1) ? -1 : 0; // tu peux faire seulement return non
}

bool formula_eval(Formula*f){
    for (size_t i=0; i < f->nclauses;i++){
        int8_t val = clause_eval(f, f->clauses[i]);
        if(val==-1||val==0){
            return false;
        }
    }
    return true;
}