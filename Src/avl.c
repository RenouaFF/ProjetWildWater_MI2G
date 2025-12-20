#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Ici, vous trouverez toutes les fonctions utilisées pour la partie histogramme du projet

int imax(int a, int b) { return (a > b) ? a : b; }
int imin(int a, int b) { return (a < b) ? a : b; }

//Fonctions AVL

donnees* copie_donnees(const donnees* src) {
    if (!src) return NULL;
    donnees* d = (donnees*)malloc(sizeof(*d));
    if (!d) return NULL;
    *d = *src;
    return d;
}

void liberer_noeud(avl* n) {
    if (!n) return;
    free(n->infos);
    free(n);
}

static avl* nouveau_noeud(const char* id, const donnees* infos) {
    avl* n = (avl*)malloc(sizeof(*n));
    if (!n) return NULL;

    memset(n, 0, sizeof(*n));
    strcpy(n->ID, id);
    n->infos = copie_donnees(infos);
    n->equilibre = 0;
    n->fg = n->fd = NULL;
    return n;
}

avl* rotationGauche(avl* a) {
    avl* pivot = a->fd;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;

    a->fd = pivot->fg;
    pivot->fg = a;

    a->equilibre     = eq_a - 1 - imax(eq_p, 0);
    pivot->equilibre = eq_p - 1 + imin(eq_a, 0);

    return pivot;
}

avl* rotationDroite(avl* a) {
    avl* pivot = a->fg;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;

    a->fg = pivot->fd;
    pivot->fd = a;

    a->equilibre     = eq_a + 1 - imin(eq_p, 0);
    pivot->equilibre = eq_p + 1 + imax(eq_a, 0);

    return pivot;
}

avl* rotationDroiteGauche(avl* a) {
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

avl* rotationGaucheDroite(avl* a) {
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}

avl* equilibrage(avl* a, int* h) {
    if (!a) return NULL;

    if (a->equilibre == 2) {
        if (a->fd && a->fd->equilibre >= 0) a = rotationGauche(a);
        else                                a = rotationDroiteGauche(a);
    } else if (a->equilibre == -2) {
        if (a->fg && a->fg->equilibre <= 0) a = rotationDroite(a);
        else                                a = rotationGaucheDroite(a);
    }
    return a;
}

avl* rechercheAVL(avl* a, const char* id) {
    while (a) {
        int c = strcmp(id, a->ID);
        if (c == 0) return a;
        a = (c < 0) ? a->fg : a->fd;
    }
    return NULL;
}

avl* insertionAVL(avl* a, const char* id, const donnees* infos, int* h) {
    if (a == NULL) {
        a = nouveau_noeud(id, infos);
        *h = (a != NULL) ? 1 : 0;
        return a;
    }

    int c = strcmp(id, a->ID);

    if (c < 0) {
        a->fg = insertionAVL(a->fg, id, infos, h);
        if (*h != 0) a->equilibre = a->equilibre - *h;
    } else if (c > 0) {
        a->fd = insertionAVL(a->fd, id, infos, h);
        if (*h != 0) a->equilibre = a->equilibre + *h; 
    } else {
        free(a->infos);
        a->infos = copie_donnees(infos);
        *h = 0;
        return a;
    }

    if (*h != 0) {
        a = equilibrage(a, h);
        if (a->equilibre == 0) *h = 0;
        else                   *h = 1;
    }

    return a;
}

void recherche(avl* racine, const char* id) {
    avl* n = rechercheAVL(racine, id);
    if (!n) {
        printf("%s : Non trouvé\n", id);
    } else {
        printf("%s : Trouvé (max=%.2f capte=%.2f traite=%.2f, eq=%d)\n",
               id,
               n->infos ? n->infos->max : 0.0,
               n->infos ? n->infos->capte : 0.0,
               n->infos ? n->infos->traite : 0.0,
               n->equilibre);
    }
}

void stocker_histo(const avl* a,FILE* fichier, int info){
    if (!a) return;
    stocker_histo(a->fd, fichier, info);
    if (a->infos) {
        if(info == 1){
            fprintf(fichier,   "%s;%.2f\n", a->ID, a->infos->max);
        }
        else if(info == 2){
            fprintf(fichier,   "%s;%.2f\n", a->ID, a->infos->capte);
        }
        else if(info == 3){
            fprintf(fichier,   "%s;%.2f\n", a->ID, a->infos->traite);
        }
    }
    stocker_histo(a->fg, fichier, info);
}
