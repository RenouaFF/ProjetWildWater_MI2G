#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre.h"

NoeudReseau *creerNoeudReseau(char *id, TypeActeur type) {
    NoeudReseau *n = malloc(sizeof(NoeudReseau));
    if (!n) return NULL;

    n->donnees = malloc(sizeof(DonneeActeur));
    if (!n->donnees) {
        free(n);
        return NULL;
    }

    strcpy(n->donnees->id, id);
    n->donnees->type = type;
    n->donnees->taux_fuite_amont = 0.0f;
    n->donnees->volume_entrant = 0.0f;

    n->parent = NULL;
    n->tete_enfants = NULL;

    return n;
}

void insertionNoeudReseau(NoeudReseau *parent, NoeudReseau *enfant) {
    if (!parent || !enfant) return;

    ListeEnfant *n = malloc(sizeof(ListeEnfant));
    n->enfant = enfant;
    n->suivant = parent->tete_enfants;
    parent->tete_enfants = n;

    enfant->parent = parent;
}

void parcoursPostfixeReseau(NoeudReseau *racine) {
    if (racine == NULL)
        return;

    ListeEnfant *courant = racine->tete_enfants;
    while (courant != NULL) {
        parcoursPostfixeReseau(courant->enfant);
        courant = courant->suivant;
    }
    printf("Acteur %s | volume entrant = %.2f\n",
           racine->donnees->id,
           racine->donnees->volume_entrant);
}

AVLReseau *rotationDroiteReseau(AVLReseau *y) {
    AVLReseau *x = y->gauche;
    AVLReseau *T2 = x->droite;

    x->droite = y;
    y->gauche = T2;

    y->equilibre = 0;
    x->equilibre = 0;

    return x;
}

AVLReseau *rotationGaucheReseau(AVLReseau *x) {
    AVLReseau *y = x->droite;
    AVLReseau *T2 = y->gauche;

    y->gauche = x;
    x->droite = T2;

    x->equilibre = 0;
    y->equilibre = 0;

    return y;
}

AVLReseau *insertionAVLReseau(AVLReseau *racine,char *id,NoeudReseau *adresse, int *h) {
    if (racine == NULL) {
        AVLReseau *n = malloc(sizeof(AVLReseau));
        strcpy(n->id, id);
        n->adresse = adresse;
        n->equilibre = 0;
        n->gauche = n->droite = NULL;
        *h = 1;
        return n;
    }

    if (strcmp(id, racine->id) < 0) {
        racine->gauche = insertionAVLReseau(racine->gauche, id, adresse, h);
        if (*h) racine->equilibre--;
    }
    else if (strcmp(id, racine->id) > 0) {
        racine->droite = insertionAVLReseau(racine->droite, id, adresse, h);
        if (*h) racine->equilibre++;
    }
    else {
        *h = 0;
        return racine;
    }

    if (racine->equilibre == -2) {
        if (racine->gauche->equilibre <= 0)
            racine = rotationDroiteReseau(racine);
    }
    else if (racine->equilibre == 2) {
        if (racine->droite->equilibre >= 0)
            racine = rotationGaucheReseau(racine);
    }

    *h = (racine->equilibre != 0);
    return racine;
}

NoeudReseau *rechercheAVLReseau(AVLReseau *racine, char *id) {
    if (racine == NULL)
        return NULL;

    int cmp = strcmp(id, racine->id);

    if (cmp == 0)
        return racine->adresse;
    else if (cmp < 0)
        return rechercheAVLReseau(racine->gauche, id);  
    else
        return rechercheAVLReseau(racine->droite, id);
}

TypeActeur type_aval(TypeActeur t){
    switch (t) {
        case N_USINE:        return N_STOCKAGE;
        case N_STOCKAGE:     return N_JONCTION;
        case N_JONCTION:     return N_RACCORDEMENT;
        case N_RACCORDEMENT: return N_USAGER;
        default:             return N_USAGER;
    }
}

