#ifndef AVL_H
#define AVL_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structures de données

typedef struct{
    double max;
    double capte;
    double traite;
} donnees;

typedef struct avl_struct{
    char ID[30];
    donnees* infos;
    int equilibre;            
    struct avl_struct* fg;
    struct avl_struct* fd;
} avl;


// Fonctions AVL 

int imax(int a, int b);
int imin(int a, int b);
donnees* copie_donnees(const donnees* src);
void liberer_noeud(avl* n);
static avl* nouveau_noeud(const char* id, const donnees* infos);
avl* rotationGauche(avl* a);
avl* rotationDroite(avl* a);
avl* rotationDroiteGauche(avl* a);
avl* rotationGaucheDroite(avl* a);
avl* equilibrage(avl* a, int* h);
avl* rechercheAVL(avl* a, const char* id);
avl* insertionAVL(avl* a, const char* id, const donnees* infos, int* h);
void recherche(avl* racine, const char* id);
void afficher_abr(const avl* a);



#endif