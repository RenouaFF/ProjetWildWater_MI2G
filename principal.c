#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Headers/avl.h"
#include "Headers/arbre.h"

#define TAILLE_MAX_LIGNE 1024
#define MAX_USINES 10000

typedef struct {
    char id[64];
    double valeur;
} entree_histo;

typedef struct ligne_struct{
    char ID[30];
    char amont[30];
    char aval[30];
    char volume[30];
    char fuite[30];
} ligne_csv;

int parse_ligne(const char *ligne, ligne_csv *res) {
    char copie[TAILLE_MAX_LIGNE];
    strcpy(copie, ligne);
    
    char *token = strtok(copie, ";");
    if (!token) return 0;
    strncpy(res->ID, token, 29);
    res->ID[29] = '\0';
    
    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(res->amont, token, 29);
    res->amont[29] = '\0';
    
    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(res->aval, token, 29);
    res->aval[29] = '\0';
    
    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(res->volume, token, 29);
    res->volume[29] = '\0';
    
    token = strtok(NULL, "\n");
    if (!token) return 0;
    strncpy(res->fuite, token, 29);
    res->fuite[29] = '\0';
    
    return 1;
}

void construire_histogramme(char* nom_fichier, avl** arbre){
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier){
        perror("Erreur ouverture fichier");
        return;
    }
    char ligne[TAILLE_MAX_LIGNE];
    int h=0;
    int nb_lignes = 0;
    int nb_usines = 0;
    int nb_sources = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        nb_lignes++;
        
        if (nb_lignes % 100000 == 0) {
            fprintf(stderr, "\rLignes: %d (usines: %d, sources: %d)", 
                    nb_lignes, nb_usines, nb_sources);
            fflush(stderr);
        }

        if (ligne[0] == '\n' || ligne[0] == '\0')
            continue;
            
        ligne_csv l;
        if (parse_ligne(ligne, &l) != 1) {
            continue;
        }
        
        if (strcmp(l.ID, "-") == 0 && 
            strcmp(l.aval, "-") == 0 && 
            strcmp(l.fuite, "-") == 0) {
            nb_usines++;
            avl* res = rechercheAVL(*arbre, l.amont);
            if (res){
                res->infos->max = atof(l.volume);
            }
            else{
                donnees d;
                d.max = atof(l.volume);
                d.capte = 0;
                d.traite = 0;
                *arbre = insertionAVL(*arbre, l.amont, &d, &h);
            }
        }
        else if (strcmp(l.ID, "-") == 0 && 
                 strcmp(l.aval, "-") != 0 && 
                 strcmp(l.volume, "-") != 0 && 
                 strcmp(l.fuite, "-") != 0) {
            nb_sources++;
            double volume_source = atof(l.volume);
            double pourcentage_fuite = atof(l.fuite);
            
            avl* res = rechercheAVL(*arbre, l.aval);
            if (res) {
                res->infos->capte += volume_source;
                res->infos->traite += volume_source * (1 - pourcentage_fuite / 100.0);
            } else {
                donnees d;
                d.max = 0;
                d.capte = volume_source;
                d.traite = volume_source * (1 - pourcentage_fuite / 100.0);
                *arbre = insertionAVL(*arbre, l.aval, &d, &h);
            }
        }
    }
    fclose(fichier);
}

// Détermination type parent
int type_parent(int type){
    if (type == N_JONCTION) return N_STOCKAGE;
    if (type == N_RACCORDEMENT) return N_JONCTION;
    if (type == N_USAGER) return N_RACCORDEMENT;
    return N_USINE;
}

// Construction arbre réseau
void parcours_fichier(char* nom_fichier, NoeudReseau** arbre, avl * usine){ 
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier){
        perror("Erreur ouverture fichier");
        return;
    }

    char ligne[TAILLE_MAX_LIGNE];
    int h = 0;

    NoeudReseau *racine = creerNoeudReseau(usine->ID, N_USINE, 0);
    *arbre = racine;

    AVLReseau *avl_reseau = NULL;
    avl_reseau = insertionAVLReseau(avl_reseau, usine->ID, racine, &h);

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\0') continue;

        ligne_csv l;
        if (!parse_ligne(ligne, &l)) continue;

        if (strcmp(l.ID, "-") == 0 && strncmp(l.aval, "Storage", 7) == 0) {
            if (strcmp(l.amont, usine->ID) == 0) {
                TypeActeur type = N_STOCKAGE;
                NoeudReseau *enfant = creerNoeudReseau(l.aval, type, atof(l.fuite));
                NoeudReseau *parent = rechercheAVLReseau(avl_reseau, l.amont);
                if (parent) {
                    avl_reseau = insertionAVLReseau(avl_reseau, l.aval, enfant, &h);
                    insertionNoeudReseau(&parent, enfant);
                }
            }
        } else if (strcmp(l.ID, "-") != 0 && strcmp(l.ID, usine->ID) == 0) {
            TypeActeur type_aval;
            if (strncmp(l.aval, "Junction", 8) == 0) type_aval = N_JONCTION;
            else if (strncmp(l.aval, "Service", 7) == 0) type_aval = N_RACCORDEMENT;
            else if (strncmp(l.aval, "Cust", 4) == 0) type_aval = N_USAGER;
            else type_aval = type_parent(N_USINE);

            NoeudReseau *noeud = rechercheAVLReseau(avl_reseau, l.aval);
            if (noeud) noeud->donnees->taux_fuite = atof(l.fuite);
            else {
                NoeudReseau *enfant = creerNoeudReseau(l.aval, type_aval, atof(l.fuite));
                NoeudReseau *parent = rechercheAVLReseau(avl_reseau, l.amont);
                if (!parent) {
                    parent = creerNoeudReseau(l.amont, type_parent(type_aval), 0);
                    avl_reseau = insertionAVLReseau(avl_reseau, l.amont, parent, &h);
                }
                avl_reseau = insertionAVLReseau(avl_reseau, l.aval, enfant, &h);
                insertionNoeudReseau(&parent, enfant);
            }
        }
    }
    fclose(fichier);
}

// Comparaison pour qsort
static int cmp(const void *a, const void *b) {
    entree_histo *ea = (entree_histo*)a;
    entree_histo *eb = (entree_histo*)b;
    if (ea->valeur < eb->valeur) return -1;
    if (ea->valeur > eb->valeur) return 1;
    return 0;
}

// Remplir tableau histogramme
void remplir_tab(avl* racine, entree_histo *tab, int *n, int mode) {
    if (!racine) return;
    remplir_tab(racine->fg, tab, n, mode);
    if (racine->infos) {
        strcpy(tab[*n].id, racine->ID);
        if (mode == 1) tab[*n].valeur = racine->infos->max;
        else if (mode == 2) tab[*n].valeur = racine->infos->capte;
        else if (mode == 3) tab[*n].valeur = racine->infos->traite;
        (*n)++;
    }
    remplir_tab(racine->fd, tab, n, mode);
}

// Libération mémoire AVL
void liberer_arbre(avl* a) {
    if (!a) return;
    liberer_arbre(a->fg);
    liberer_arbre(a->fd);
    free(a->infos);
    free(a);
}

// Libération mémoire NoeudReseau
void liberer_noeud_reseau(NoeudReseau* n) {
    if (!n) return;
    for (int i = 0; i < n->nb_enfants; i++)
        liberer_noeud_reseau(n->enfants[i]);
    free(n->donnees);
    free(n->enfants);
    free(n);
}

// Main
int main(int argc, char *argv[]) {
    if (argc != 4) return EXIT_FAILURE;

    char *fichier = argv[1];
    char *action  = argv[2];
    char *valeur  = argv[3];

    avl *racine = NULL;
    construire_histogramme(fichier, &racine);

    if (strcmp(action, "histo") == 0) {
        entree_histo tab[MAX_USINES];
        int n = 0;
        int mode = 0;

        if (strcmp(valeur, "max") == 0) {
            mode = 1;
        } else if (strcmp(valeur, "src") == 0) {
            mode = 2;
        } else if (strcmp(valeur, "real") == 0) {
            mode = 3;
        } else return EXIT_FAILURE;

        remplir_tab(racine, tab, &n, mode);
        qsort(tab, n, sizeof(entree_histo), cmp);

        char filename[64];
        sprintf(filename, "histo_%s.dat", valeur);
        FILE *out = fopen(filename, "w");
        if (!out) return EXIT_FAILURE;
        
        if (mode == 1) fprintf(out, "identifier;max volume (k.m3.year-1)\n");
        else if (mode == 2) fprintf(out, "identifier;source volume (k.m3.year-1)\n");
        else if (mode == 3) fprintf(out, "identifier;real volume (k.m3.year-1)\n");
        
        // Les 50 plus petites
        for (int i = 0; i < 50 && i < n; i++)
            fprintf(out, "%s;%.3f\n", tab[i].id, tab[i].valeur);
        
        // Les 10 plus grandes
        for (int i = (n > 10 ? n - 10 : 0); i < n; i++)
            fprintf(out, "%s;%.3f\n", tab[i].id, tab[i].valeur);
        
        fclose(out);

        liberer_arbre(racine);
        return EXIT_SUCCESS;
    }

    if (strcmp(action, "leaks") == 0) {
        FILE *f = fopen("rendements.dat", "a");
        if (!f) return EXIT_FAILURE;

        avl *usine = rechercheAVL(racine, valeur);
        if (!usine) {
            fprintf(f, "%s;-1\n", valeur);
            fclose(f);
            liberer_arbre(racine);
            return EXIT_SUCCESS;
        }

        NoeudReseau *arbre = NULL;
        parcours_fichier(fichier, &arbre, usine);
        double fuite = cumul_fuite(arbre, usine->infos->traite);
        fprintf(f, "%s;%.3f\n", usine->ID, fuite);

        fclose(f);
        liberer_noeud_reseau(arbre);
        liberer_arbre(racine);
        return EXIT_SUCCESS;
    }

    liberer_arbre(racine);
    return 0;
}
