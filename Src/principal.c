#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "avl.c"
#include "arbre.h"
#include "arbre.c"

#define TAILLE_MAX_LIGNE 1024

//Structure de donnée

typedef struct ligne_struct{
    char ID[30];
    char amont[30];
    char aval[30];
    char volume[30];
    char fuite[30];
}ligne_csv;

//Converti une chaine de caractere en structure ligne_csv

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

void construire_histogramme(char* nom_fichier, avl** arbre){
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier){
        perror("Erreur ouverture fichier");
        return;
    }

    char ligne[TAILLE_MAX_LIGNE];
    int h=0;

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
            d->max = atof(l.volume);
            d->capte = 0;
            d->traite = 0;
            *arbre= insertionAVL(*arbre, l.amont, d, &h);
        
        //Ligne = src->usine    
        }else if (strcmp(l.ID, "-") == 0 && strcmp(l.volume, "-") != 0) {
            double volume_source = atof(l.volume);
            double pourcentage_fuite = atof(l.fuite);
            avl* res= rechercheAVL(*arbre, l.aval);
            if (res) {
                res->infos->capte += volume_source;
                res->infos->traite += volume_source * (1 - pourcentage_fuite / 100.0);
            }
        }
    }
}

void parcours_fichier(char* nom_fichier, NoeudReseau** arbre, avl * usine){ 

    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier){
        perror("Erreur ouverture fichier");
        return;
    }

    char ligne[TAILLE_MAX_LIGNE];
    int h=0;

    NoeudReseau *racine = creerNoeudReseau(usine->ID, N_USINE,0);
    *arbre = racine;

    AVLReseau *avl_reseau = NULL;
    avl_reseau = insertionAVLReseau(avl_reseau, usine->ID, racine, &h);

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\0')
            continue;
        ligne_csv l;
        if (parse_ligne(ligne, &l)==0) {
            break;
        }
        if (strcmp(l.ID, "-") == 0 && strncmp(l.aval, "Storage", 7) == 0) {
            if (strcmp(l.amont, usine->ID) == 0) {
                TypeActeur type = N_STOCKAGE;
                NoeudReseau *enfant = creerNoeudReseau(l.aval, type, atof(l.fuite));
                NoeudReseau *parent = rechercheAVLReseau(avl_reseau, l.amont);
                if(parent){
                    avl_reseau = insertionAVLReseau(avl_reseau, l.aval, enfant, &h);
                    insertionNoeudReseau(&parent, enfant);
                }
            }
        }
        else if (strcmp(l.ID, "-") != 0 && strcmp(l.ID, usine->ID) == 0) {
    
            TypeActeur type_aval;
            
            if (strncmp(l.aval, "Junction", 8) == 0) {
                type_aval = N_JONCTION;
            } 
            else if (strncmp(l.aval, "Service", 7) == 0) {
                type_aval = N_RACCORDEMENT;
            } 
            else if (strncmp(l.aval, "Cust", 4) == 0) {
                type_aval = N_USAGER;
            }
            
            NoeudReseau *parent = rechercheAVLReseau(avl_reseau, l.amont);
            if(parent){
                NoeudReseau *enfant = creerNoeudReseau(l.aval, type_aval, strtod(l.fuite, 0));
                avl_reseau = insertionAVLReseau(avl_reseau, l.aval, enfant, &h);
                insertionNoeudReseau(&parent, enfant);
            }
        }
    }
    fclose(fichier);
}



int main(int argc, char *argv[]) {
    avl *racine = NULL;
    NoeudReseau *arbre=NULL;
    construire_histogramme("c-wildwater_v0.dat", &racine);
    if(argc == 3){
        if(strcmp(argv[1], "hist")==0){
            if(strcmp(argv[2], "max")==0){
                FILE* fichier    = fopen("vol_max.dat", "w");
                if (!fichier) {
                    perror("Erreur ouverture fichiers sortie");
                    if (fichier) fclose(fichier);
                    return 0;
                }
                fprintf(fichier,    "id_usine;volume_max\n");
                stocker_histo(racine, fichier, 1);
            }
            else if(strcmp(argv[2], "src")==0){
                FILE* fichier    = fopen("vol_capte.dat", "w");
                if (!fichier) {
                    perror("Erreur ouverture fichiers sortie");
                    if (fichier) fclose(fichier);
                    return 0;
                }
                fprintf(fichier,  "id_usine;volume_capte\n");
                stocker_histo(racine, fichier, 2);
            }
            else if(strcmp(argv[2], "real")==0){
                FILE* fichier    = fopen("vol_traite.dat", "w");
                if (!fichier) {
                    perror("Erreur ouverture fichiers sortie");
                    if (fichier) fclose(fichier);
                    return 0;
                }
                fprintf(fichier,    "id_usine;volume_traite\n");
                stocker_histo(racine, fichier, 3);
            }
        }
        else if (strcmp(argv[1], "leaks")==0){
            if(strcmp(argv[2], "")!=0){
                avl* usine= rechercheAVL(racine, argv[2]);
                if(usine){
                    parcours_fichier("data.dat", &arbre, usine);
                    double fuite=cumul_fuite(arbre, usine->infos->traite);
                    FILE* f = fopen("rendements.dat", "a");
                    if (!f){
                        perror("Erreur ouverture rendements.dat");
                        return 0;
                    }
                    fprintf(f, "%s;%.3f\n", usine->ID, fuite);
                    fclose(f);
                }
            }
        }
        else{
            printf("erreur");
        }

    }
    return 0;
}
