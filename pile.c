#include <stdlib.h>
#include <stdio.h>

const int MAX = 5;

typedef int tub_data[5];

typedef struct pile{
    tub_data tub_data;
    int sommet;
    int nb;
} Pile;

Pile maPile;

void push(Pile *ref, int valeur);
int STK_FULL(Pile ref);
int STK_EMPTY(Pile ref);
void pop(Pile *ref, int *valeur);
int main(){
    
    
    return 0;
}

void push(Pile *ref, int valeur){
    ref->sommet++;
    ref->tub_data[ref->sommet] = valeur;
    ref->nb++;
}

int STK_FULL(Pile ref){
    if(ref.nb == MAX)
        return 1;
    else
        return 0;
}

int STK_EMPTY(Pile ref){
    if(ref.nb == 0 && ref.sommet == 0)
        return 1;
    else
        return 0;
}

void pop(Pile *ref, int *valeur){
    *valeur = ref->tub_data[ref->sommet];
    ref->sommet--;
    ref->nb--;
}




