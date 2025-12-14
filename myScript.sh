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


stop=$(date +%s)
duree=$((stop - start))
echo "Durée : $duree secondes"
