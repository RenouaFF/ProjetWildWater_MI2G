# 📊 Projet C-wildwater

## 📌 Description du projet

Ce projet est un programme permettant de faire la
synthèse de données d’un système de distribution d’eau.

* un **programme en C** (`principal`)
* un **script Bash** d’automatisation
* **gnuplot** pour la génération de graphiques

Deux types d’analyses sont possibles :

* **Histogrammes (`histo`)**
* **Fuites (`leaks`)**

Les résultats sont filtrés, triés et visualisés automatiquement.

---

## 📂 Organisation du projet

```
.
├── Makefile
├── principal          # Programme C compilé
├── script.sh          # Script Bash principal
├── tmp/               # Fichiers temporaires (csv, dat, scripts gnuplot)
├── graphs/            # Graphiques générés (.png)
└── README.md
```

---

## ⚙️ Prérequis

### 🧰 Outils nécessaires

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

## 2 - ▶️ Compilation

1) cloner le projet
```bash
git clone https://github.com/RenouaFF/ProjetWildWater_MI2G.git
cd ProjetWildWater_MI2G
```

2) rendre le script executable 
```bash
chmod +x myScript.sh
```
---

### 📌 Syntaxe générale

```bash
./prog <fichier_données> <mode> <type>
```

---

## 🔹 <mode> `histo` (Histogrammes)

### 📌 Syntaxe

```bash
./prog c-wildwater_v3.dat histo <type>
```

### 📊 <types> disponibles

* `src`  → histogramme des sources
* `max`  → valeurs maximales
* `real` → valeurs réelles

### 📁 Résultats générés

* Histogramme des **50 plus petites usines**
* Histogramme des **10 plus grandes usines**

📂 Stockés dans :

```text
graphs/
├── histo_<type>_petit.png
└── histo_<type>_grand.png
```

---

## 🔹 <mode> `leaks` (Fuites)

### 📌 Syntaxe

```bash
./prog c-wildwater_v3.dat leaks <identifiant_usine>
```

### 📁 Résultat

* Génération du fichier `leaks.dat`
* Stocké dans le dossier `tmp/`

---

## Gestion des fichiers temporaires

Les fichiers intermédiaires sont automatiquement déplacés dans :

```text
tmp/
```

Cela inclut :

* fichiers `.csv`
* fichiers `.dat`
* scripts gnuplot `.gp`

---

## ⏱️ Temps d’exécution

Le script mesure automatiquement le temps total d’exécution :

```text
Durée : X secondes
```
