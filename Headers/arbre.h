#ifndef ARBRE_H
#define ARBRE_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
    N_USINE,        
    N_STOCKAGE,     
    N_JONCTION,     
    N_RACCORDEMENT, 
    N_USAGER        
} TypeActeur;

//Structure pour stocker les donnees d'un acteur du reseau

typedef struct {
    char id[50];                
    TypeActeur type;            
    float taux_fuite;      
} DonneeActeur;

//Structure pour un noeud de l'arbre de distribution du reseau.

typedef struct noeud_reseau {
    DonneeActeur *donnees;
    struct noeud_reseau *parent;
    struct noeud_reseau **enfants;
    int nb_enfants;
} NoeudReseau;

typedef struct avl_reseau {
    char id[50];
    NoeudReseau *adresse;
    int equilibre;
    struct avl_reseau *gauche;
    struct avl_reseau *droite;
} AVLReseau;

//Fonctions avl et noeud reseau

NoeudReseau *creerNoeudReseau(char *id, TypeActeur type, double taux_fuite);
void insertionNoeudReseau(NoeudReseau **parent, NoeudReseau *enfant);
void parcoursPostfixeReseau(NoeudReseau *racine, int profondeur) ;
AVLReseau *rotationDroiteReseau(AVLReseau *y);
AVLReseau *rotationGaucheReseau(AVLReseau *x);
AVLReseau *insertionAVLReseau(AVLReseau *racine,char *id,NoeudReseau *adresse, int *h);
NoeudReseau *rechercheAVLReseau(AVLReseau *racine, char *id);
TypeActeur type_aval(TypeActeur t);
float cumul_fuite(NoeudReseau* noeud, float v_arrive);

#endif
