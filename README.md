# Projet C-wildwater

## Sommaire
- Description du projet
- Organisation du projet
- Prérequis
  - Installation de gnuplot
  - Compilation et exécution
    - Modes d’analyse
    - Mode histo (Histogrammes)
    - Mode leaks (Fuites)
- Temps d’exécution
- Contributeurs

# Description du projet

Ce projet est un programme permettant de faire la
synthèse de données d’un système de distribution d’eau.

* un **programme en C** (`principal`)
* un **script Bash** d’automatisation
* **gnuplot** pour la génération de graphiques

Deux types d’analyses sont possibles :

* **Histogrammes (`hist`)**
* **Fuites (`leaks`)**

Les résultats sont filtrés, triés et visualisés automatiquement.

---

## Organisation du projet (après execution)

```
├── Headers/
│   ├── avl.h
│   └── arbre.h
├── Src/
│   ├── avl.c
│   └── arbre.c
├── graphs/
├── test (version v0)/
├── test v3/
├── Makefile
├── c-wildwater_v0.dat
├── principal.c
├── script.sh
├── rendements.dat
├── histo_(max/src/real).dat
├── Rapport
└── README.md
```

---

# Prérequis

### 1 - Installation de gnuplot

```bash
sudo apt update
sudo apt install gnuplot-nox
```

Vérification :

```bash
gnuplot --version
```
---

## 2 - Compilation

1) cloner le projet
```bash
git clone https://github.com/RenouaFF/ProjetWildWater_MI2G.git
cd ProjetWildWater_MI2G
```

2) rendre le script executable 
```bash
chmod +x script.sh
```
---

3) exécution 

```bash
./script.sh <action> <valeur>
```
---

## `<action>` `hist` (Histogrammes)

```bash
./script.sh hist <valeur>
```

### `<valeur>` disponibles

* `src`  → selon les volumes captés des usines
* `max`  → selon les capacités maximales des usines
* `real` → selon les volumes traités des usines

### Résultats générés

* Histogramme des usines selon **src/max/real**
* Histogramme des **50 plus petites usines**
* Histogramme des **10 plus grandes usines**


## `<action>` `leaks` (Fuites)

```bash
./myScript.sh leaks <identifiant_usine>
```

### Résultat

* Génération du fichier `rendements.dat`

---

## Temps d’exécution

Le script mesure automatiquement le temps total d’exécution :

```text
Durée : X secondes
```
--- 

## Contributeurs 

Groupe MI2-G :
Belquass Hajar
Xerri Sarah
Fassi fehri Renoua
