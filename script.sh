#!/bin/bash

start_ms=$(date +%s%3N)

# Vérification nombre d'arguments
if [ $# -ne 3 ]; then
    echo "Erreur : usage attendu :"
    echo "$0 <fichier_donnees> histo {max|src|real}"
    echo "$0 <fichier_donnees> leaks \"ID_USINE\""
    exit 1
fi

DATAFILE="$1"
ACTION="$2"
VALUE="$3"

# Vérification fichier de données
if [ ! -f "$DATAFILE" ]; then
    echo "Erreur : fichier de données introuvable"
    exit 1
fi

# Vérification Makefile
if [ ! -f "Makefile" ]; then
    echo "Erreur : Makefile introuvable"
    exit 1
fi

# Création dossiers
mkdir -p graphs

# Vérification gnuplot
if ! command -v gnuplot >/dev/null 2>&1; then
    echo "Erreur : gnuplot non installé"
    exit 1
fi

# Compilation conditionnelle
if [ ! -f "prog" ]; then
    echo "Compilation du programme C..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur : compilation échouée"
        exit 1
    fi
fi

# Exécution programme C
if [ "$ACTION" = "histo" ]; then
    if [[ "$VALUE" != "max" && "$VALUE" != "src" && "$VALUE" != "real" ]]; then
        echo "Erreur : histo nécessite max | src | real"
        exit 1
    fi
    ./prog "$DATAFILE" histo "$VALUE"
elif [ "$ACTION" = "leaks" ]; then
    ./prog "$DATAFILE" leaks "$VALUE"
else
    echo "Erreur : action invalide"
    exit 1
fi

# Vérification code retour
if [ $? -ne 0 ]; then
    echo "Erreur : échec du programme C"
    exit 1
fi

# Génération graphique histo
if [ "$ACTION" = "histo" ]; then
    SRCFILE="histo_${VALUE}.dat"

    if [ ! -f "$SRCFILE" ]; then
        echo "Erreur : fichier $SRCFILE non généré"
        exit 1
    fi

    # Graphique des 50 plus PETITES
    gnuplot << EOF
set terminal png size 1400,800
set output "graphs/histo_${VALUE}_min50.png"
set datafile separator ";"
set style data histograms
set style fill solid
set boxwidth 0.8
set xtics rotate by -45 font ",7"
set ylabel "Volume (k.m3.year-1)"
set title "50 plus petites usines - ${VALUE}"
set key off
plot "$SRCFILE" every ::1::50 using 2:xtic(1) with boxes notitle
EOF

    # Graphique des 10 plus GRANDES
    gnuplot << EOF
set terminal png size 1000,600
set output "graphs/histo_${VALUE}_max10.png"
set datafile separator ";"
set style data histograms
set style fill solid
set boxwidth 0.8
set xtics rotate by -45 font ",9"
set ylabel "Volume (k.m3.year-1)"
set title "10 plus grandes usines - ${VALUE}"
set key off
plot "$SRCFILE" every ::51::60 using 2:xtic(1) with boxes notitle
EOF

    if [ -f "graphs/histo_${VALUE}_min50.png" ] && [ -f "graphs/histo_${VALUE}_max10.png" ]; then
        echo "Graphiques générés :"
        echo "  - graphs/histo_${VALUE}_min50.png"
        echo "  - graphs/histo_${VALUE}_max10.png"
    else
        echo "Erreur : échec génération graphique"
        exit 1
    fi
fi

end_ms=$(date +%s%3N)
echo "Durée totale : $((end_ms - start_ms)) ms"
