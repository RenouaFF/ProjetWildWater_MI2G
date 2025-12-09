#include <stdio.h>
#include <stdlib.h>

typedef struct{
    double max;
    double src;
    double real;
}donnees;

typedef struct avl_struct{
    char* ID;
    donnees* infos;
    int equilibre;
    struct avl_struct* fg;
    struct avl_struct* fd
}avl;

typedef struct chainon_struct {
    avl* arbre;
    struct chainon_struct* suivant;
} chainon;

typedef struct file_struct {
    chainon* tete;
    chainon* queue;
} file;

void enfiler(file** pfile, avl* arbre)
{
    chainon* nouveau = malloc(sizeof(chainon));
    if (nouveau == NULL) {
        exit(1);
    }

    nouveau->arbre = arbre;
    nouveau->suivant = NULL;

    if ((*pfile)->tete == NULL) {
        (*pfile)->tete = nouveau;
        (*pfile)->queue = nouveau;
    } else {
        (*pfile)->queue->suivant = nouveau;
        (*pfile)->queue = nouveau;
    }
}

avl* defiler(file** pfile)
{
    if ((*pfile)->tete == NULL) {
        return NULL;
    }

    chainon* tmp = (*pfile)->tete;
    avl* a = tmp->arbre;

    (*pfile)->tete = (*pfile)->tete->suivant;

    if ((*pfile)->tete == NULL) {
        (*pfile)->queue = NULL;
    }

    free(tmp);
    return a;
}

avl* rotationG(avl* a){
    avl* pivot = a->fd;
    int eq_a = a->equilibre;
    int eq_pivot = pivot->equilibre;

    a->fd = pivot->fg;
    pivot->fg = a;

    a->equilibre = eq_a - 1 - (eq_pivot > 0 ? eq_pivot : 0);
    pivot->equilibre = eq_pivot - 1 + (eq_a < 0 ? eq_a : 0);

    return pivot;
}

avl* rotationD(avl* a){
    avl* pivot = a->fg;
    int eq_a = a->equilibre;
    int eq_pivot = pivot->equilibre;

    a->fg = pivot->fd;
    pivot->fd = a;

    a->equilibre = eq_a + 1 - (eq_pivot < 0 ? eq_pivot : 0);
    pivot->equilibre = eq_pivot + 1 + (eq_a > 0 ? eq_a : 0);

    return pivot;
}

avl* rotationDG(avl* a){
    a->fd = rotationD(a->fd);
    return rotationG(a);
}

avl* rotationGD(avl* a){
    a->fg = rotationG(a->fg);
    return rotationD(a);
}

avl* equilibrage(avl* a){
    if (a->equilibre == 2){
        if (a->fd->equilibre >= 0){
            a = rotationG(a);
        }
        else{
            a = rotationDG(a);
        }
    }else if (a->equilibre == -2){
        if (a->fg->equilibre <= 0){
            a = rotationD(a);
        }else{
            a = rotationGD(a);
        }
    }
    return a;
}

avl* insertionAVL(avl* a, char* e, donnees* d, int* h){
    if (a == NULL){
        a = malloc(sizeof(avl));
        if (a == NULL) exit(1);
        strcpy(a->ID, e); 
        a->infos = d;
        a->equilibre = 0;
        a->fg = NULL;
        a->fd = NULL;
        *h = 1;
        return a;
    }
    int cmp = strcmp(e, a->ID);
    if (cmp < 0) {
        a->fg = insertionAVL(a->fg, e, d, h);
        if (*h != 0)
            a->equilibre -= *h;
    }else if (cmp > 0) {
        a->fd = insertionAVL(a->fd, e, d, h);
        if (*h != 0)
            a->equilibre += *h;
    }else {
        *h = 0;
        return a;
    }
    if (*h != 0) {
        a = equilibrage(a);
        if (a->equilibre == 0)
            *h = 0;
        else
            *h = 1;
    }
    return a;
}

int recherche(avl* a, const char* e){
    if (a == NULL) return 0;

    int cmp = strcmp(e, a->ID);

    if (cmp == 0) return 1;

    if (cmp < 0) return recherche(a->fg, e);

    return recherche(a->fd, e);
}

void largeur(avl* a){
    if (a == NULL) return;

    file* file = malloc(sizeof(file));
    if (file == NULL) return;

    file->tete = NULL;
    file->queue = NULL;
    enfiler(&file, a);
    
    while (file->tete != NULL) {
        avl* tmp = defiler(&file);
        printf("%s ", tmp->ID);
        if (tmp->fg) enfiler(&file, tmp->fg);
        if (tmp->fd) enfiler(&file, tmp->fd);
    }
    free(file);
}

void prefixe(avl* a){
    if (a == NULL)return;
    printf("%s ", a->ID);
    prefixe(a->fg);
    prefixe(a->fd);
}

void infixe(avl* a){
    if (a == NULL) return;
    infixe(a->fg);
    printf("%s ", a->ID);
    infixe(a->fd);
}

void suffixe(avl* a){
    if (a == NULL) return;
    suffixe(a->fg);
    suffixe(a->fd);
    printf("%s ", a->ID);
}

