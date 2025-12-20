#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre.h"

NoeudReseau *creerNoeudReseau(char *id, TypeActeur type, double taux_fuite) {
    NoeudReseau *n = malloc(sizeof(NoeudReseau));
    if (!n) return NULL;

    n->donnees = malloc(sizeof(DonneeActeur));
    if (!n->donnees) {
        free(n);
        return NULL;
    }

    strcpy(n->donnees->id, id);
    n->donnees->type = type;
    n->donnees->taux_fuite = taux_fuite;

    n->parent = NULL;
    n->enfants = NULL;
    n->nb_enfants=0;
    return n;
}

void insertionNoeudReseau(NoeudReseau **parent, NoeudReseau *enfant) {
    if (parent == NULL || *parent == NULL || enfant == NULL) return;

    NoeudReseau *p = *parent;
    enfant->parent = p;

    // Agrandir le tableau des enfants de 1
    NoeudReseau **nv = realloc(p->enfants, (p->nb_enfants + 1) * sizeof(NoeudReseau));
    if (!nv) {
        // échec realloc => ne pas modifier la structure
        enfant->parent = NULL;
        return;
    }

    p->enfants = nv;
    p->enfants[p->nb_enfants] = enfant;
    p->nb_enfants += 1;
}

void parcoursPostfixeReseau(NoeudReseau *racine, int profondeur) {
    if (racine == NULL)
        return;
    
    for (int i = 0; i < profondeur; ++i) printf("  ");  // indentation
    printf("%s (type=%d, fuite=%.4f, enfants=%d)\n",
           racine->donnees->id,
           (int)racine->donnees->type,
           racine->donnees->taux_fuite,
           racine->nb_enfants);

    for(int i=0; i<racine->nb_enfants; i++){
        parcoursPostfixeReseau(racine->enfants[i], profondeur + 1);
    }
    //printf("%s", racine->donnees->id);
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

float cumul_fuite(NoeudReseau* noeud, float v_arrive){
    if(!noeud) return 0;

    float fuite=v_arrive*(noeud->donnees->taux_fuite/100);
    float v_restant=v_arrive-fuite;
    if(noeud->nb_enfants==0 || noeud->donnees->type==N_USAGER){
        return fuite;
    }
    float v_par_enfant=v_restant/(noeud->nb_enfants);
    float total_fuite=fuite;
    for(int i=0; i<(noeud->nb_enfants);i++){
        total_fuite+=cumul_fuite(noeud->enfants[i],v_par_enfant);
    }
    return total_fuite;
}

