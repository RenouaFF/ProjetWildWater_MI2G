#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "avl.c"
#include "arbre.h"
#include "arbre.c"


#define SEPARATEUR ";"
#define TAILLE_MAX_LIGNE 1024

//Structure de donnée

typedef struct ligne_struct{
    char ID[30];
    char amont[30];
    char aval[30];
    char volume[30];
    char fuite[30];
}ligne_csv;

//Convertir une chaine de caractere en structure ligne_csv

int parse_ligne(const char *ligne, ligne_csv *res) {
    if (sscanf(ligne,
               "%29[^;];%29[^;];%29[^;];%29[^;];%29[^;\n]",
               res->ID,
               res->amont,
               res->aval,
               res->volume,
               res->fuite) == 5) {
        return 1; 
    }
    return 0;
}

// Parcourir le fichier et remplir avl avec les infos de l'usine 

void parcours_fichier(char* nom_fichier, avl** arbre, AVLReseau** avl_reseau){ 

    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier){
        perror("Erreur ouverture fichier");
        return;
    }

    char ligne[TAILLE_MAX_LIGNE];
    int h;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\0')
            continue;
        ligne_csv l;
        if (parse_ligne(ligne, &l)==0) {
            break;
        }
        //Ligne = usine
        if (strcmp(l.ID, "-") == 0 && strcmp(l.aval, "-") == 0) {
            donnees* d = malloc(sizeof(donnees));
            d->max = atof(l.volume); // atof : convertir chaine de caractère en val numérique
            d->capte = 0;
            d->traite = 0;
            *arbre= insertionAVL(*arbre, l.amont, d, &h);
        
        //Ligne = src->usine    
        }else if (strcmp(l.ID, "-") == 0 && strcmp(l.amont, "-") != 0 && strcmp(l.aval, "-") != 0) { 
            double volume_source = atof(l.volume);
            double pourcentage_fuite = atof(l.fuite);
            //Mise à jour des informations de l'usine si elle existe déjà
            avl* res= rechercheAVL(*arbre, l.aval);
            if (res) {
                res->infos->capte += volume_source;
                res->infos->traite += volume_source * (1 - pourcentage_fuite / 100.0);
            }
            //Construction réseau 
        }if (strcmp(l.ID, "-") == 0 && strcmp(l.amont, "-") != 0 && strcmp(l.aval, "-") == 0) {
            int h2 = 0;
            NoeudReseau *usine = rechercheAVLReseau(*avl_reseau, l.amont);

            if (usine == NULL) {
                usine = creerNoeudReseau(l.amont, N_USINE);
                usine->donnees->volume_entrant = atof(l.volume);
                *avl_reseau = insertionAVLReseau(*avl_reseau, l.amont, usine, &h2);
            }
        }else if (strcmp(l.amont, "-") != 0 && strcmp(l.aval, "-") != 0) {
            int h2 = 0;
            // Recherche du parent (si déjà existant) et création de l'enfant
            NoeudReseau *parent = rechercheAVLReseau(*avl_reseau, l.amont);
            if (parent == NULL) continue;
            NoeudReseau *enfant = rechercheAVLReseau(*avl_reseau, l.aval);
            if (enfant == NULL) {
                TypeActeur type_enfant = type_aval(parent->donnees->type);
                enfant = creerNoeudReseau(l.aval, type_enfant);
                *avl_reseau = insertionAVLReseau(*avl_reseau, l.aval, enfant, &h2);
            }
            insertionNoeudReseau(parent, enfant);
            //Mise à jour des fuites
            if (strcmp(l.fuite, "-") != 0){
                enfant->donnees->taux_fuite_amont = atof(l.fuite);
            }
        }
    }
    fclose(fichier);
}

int main() {
    avl *racine = NULL;
    AVLReseau *avl_reseau = NULL;
    parcours_fichier("c-wildwater_v0.dat", &racine, &avl_reseau);
    printf("\nArbre des usines (AVL) après insertions :\n");
    afficher_abr(racine);
    printf("\nParcours postfixe du réseau :\n");
    if (avl_reseau != NULL) {
        NoeudReseau *racine_reseau = avl_reseau->adresse;
        parcoursPostfixeReseau(racine_reseau);
    }
    return 0;
}
