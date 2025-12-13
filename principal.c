#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

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

void parcours_fichier(char* nom_fichier, avl** arbre){ 

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
        }
    }
    fclose(fichier);
}

int main(void) {
    avl* racine = NULL;
    int h = 0;

    parcours_fichier("data.dat",&racine);
    
    printf("\nArbre après insertions \n");
    afficher_abr(racine);

    return 0;
}