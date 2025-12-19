#!/bin/bash

start=$(date +%s) #pour mémorier l'heure de départ, fonction faites à l'aide de chatgbt

echo "nombre d'arguments : " $# # 3 arguments
echo "Les arguments sont : " $* # chemain du fichier, histo ou leaks, max/src/real ou identifiant
echo "Le second argument est : " $2 #histo ou leaks
#echo "Le code de retour du dernier echo est : " $? 

# si le nombre d'argument est different de 3 donc c faux (pas + ni -)
if [ $# -ne 3 ]; then
    echo "Erreur : nombre d'arguments incorrect, veuillez entrer trois arguments."
    exit 1
fi    

# verifier si le Makefile existe
if [ ! -f "Makefile" ];
then
    echo "Erreur: Le fichier Makefile est introuvable."
    exit 1
fi

# mémoriser le dossier courant
ROOT_DIR=$(pwd)

# créer tmp s'il n'existe pas
if [ ! -d "tmp" ]; then
    mkdir tmp
fi

# créer graphs s'il n'existe pas
if [ ! -d "graphs" ]; then
    mkdir graphs
fi

#verif gnuplot installe
if ! command -v gnuplot >/dev/null 2>&1; then
    echo "Erreur : gnuplot n'est pas installé"
    exit 1
fi

# verifier si le C existe
#if [ ! -f principal ]; then
#   echo "Erreur: le fichier prog n'existe pas"
#   exit 1
#else
    echo "Compilation du C en cours ..."
    make
#fi    
  
# verifier si la compilation a réussi 
if [ $? -ne 0 ]; then
    echo "Erreur: échec"
    exit 1
fi
 
# teste si le fichier existe et que c’est un fichier régulier (pas un dossier).
if [ ! -f "$1" ]; then 
    echo "Erreur: fichier introuvable" 
    exit 1
fi

#arg 2 doit etre soit histo soit leaks et rien d'autre 
if [ "$2" != "histo" ] && [ "$2" != "leaks" ]; then
    echo "Erreur: Veuillez saisir: histo ou leaks"
    exit 1 
fi 

# si arg 2 est histo, voici les conditions à suivre
if [ "$2" = "histo" ]; then
    if [ -z "$3" ]; then
        echo "Erreur, l'argument 3, ne peux pas être vide"
        exit 1
    elif [ "$3" != "src" ] && [ "$3" != "max" ] && [ "$3" != "real" ]; then
        echo "Erreur, mauvais type d'histo, veuillez saisir: max, src ou real"
        exit 2
    fi
    ./principal "$1" "histo" "$3"
    retour=$?
fi    

# si arg 3 est leaks, voici les conditions à suivre
if [ "$2" = "leaks" ]; then
    if [ -z "$3" ]; then
        echo "Erreur, l'argument 3, ne peux pas être vide"
        exit 1
    fi    
        echo "le troisième argument est l'identifiant de l'usine."
        ./principal "$1" "leaks" "$3"
        retour=$?
fi

# si le retour est different de 0, donc Erreur
if [ $retour -ne 0 ]; then
    echo "Erreur: échec"
    exit 1
fi

#creation csv
if [ "$2" = "histo" ] && [ -f "histo_$3.csv" ]; then
    mv "histo_$3.csv" tmp/
fi

if [ "$2" = "leaks" ] && [ -f "leaks.dat" ]; then
    mv "leaks.dat" tmp/
fi

#flitrage
if [ "$2" = "histo" ]; then
	tail -n +2 "tmp/histo_$3.csv" > "tmp/histo_${3}_filtered.csv"

	# tri par valeur (colonne 2)
	sort -t ';' -k2 -n "tmp/histo_${3}_filtered.csv" > "tmp/histo_${3}_sorted.csv"

	# 50 plus petites usines
	head -n 50 "tmp/histo_${3}_sorted.csv" > "tmp/histo_${3}_petit.csv"

	# 10 plus grandes usines
	tail -n 10 "tmp/histo_${3}_sorted.csv" > "tmp/histo_${3}_grand.csv"

cat > "tmp/plot_${3}_petit.gp" << EOF
set terminal png size 1000,600
set output "graphs/histo_${3}_petit.png"
set datafile separator ";"
set style data histograms
set style fill solid
set boxwidth 0.8
set xtics rotate by -45
set title "Histogramme $3 - 50 plus petites usines"
plot "tmp/histo_${3}_petit.csv" using 2:xtic(1) notitle
EOF

gnuplot "tmp/plot_${3}_petit.gp"

cat > "tmp/plot_${3}_big.gp" << EOF
set terminal png size 1000,600
set output "graphs/histo_${3}_grand.png"
set datafile separator ";"
set style data histograms
set style fill solid
set boxwidth 0.8
set xtics rotate by -45
set title "Histogramme $3 - 10 plus grandes usines"
plot "tmp/histo_${3}_grand.csv" using 2:xtic(1) notitle
EOF

gnuplot "tmp/plot_${3}_grand.gp"

fi

cd "$ROOT_DIR"

stop=$(date +%s)
duree=$((stop - start))
echo "Durée : $duree secondes"
