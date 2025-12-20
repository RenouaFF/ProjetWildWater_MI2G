#!/bin/bash
set -euo pipefail

start=$(date +%s)

action="${1:-}"
valeur="${2:-}"

# Vérifier action
if [[ -z "$action" ]]; then
  echo "Usage: $0 {hist|leaks} <valeur>"
  exit 1
fi

if [[ "$action" == "hist" ]]; then
  if [[ -z "$valeur" ]]; then
    echo "Erreur: hist nécessite une valeur {max|src|real}"
    exit 1
  fi

  # Choisir fichier + image selon valeur
  if [[ "$valeur" == "max" ]]; then
    donnee="vol_max.dat"
  elif [[ "$valeur" == "src" ]]; then
    donnee="vol_capte.dat"
  elif [[ "$valeur" == "real" ]]; then
    donnee="vol_traite.dat"
  else
    echo "Erreur: valeur hist invalide: '$valeur' (attendu: max|src|real)"
    exit 1
  fi

  # Exécuter le programme C
  ./principal "hist" "$valeur"

  # Vérifier que le fichier de données existe
  if [[ ! -f "$donnee" ]]; then
    echo "Erreur: fichier de données introuvable: $donnee"
    exit 1
  fi

  tail -n +2 "$donnee" | sort -t';' -k2,2n > triees.dat
  {
    head -n 1 "$donnee"
    tail -n 10 triees.dat
  } > histo.txt
  rm triees.dat
  awk -F';' 'NR>1 {print "\"" $1 "\"",$2}' histo.txt > data.dat
  # Générer le bar chart
  gnuplot << EOF
set terminal png size 1000,600
set output "histo_plus_grandes.png"

set title "Volume par usine ($valeur)"
set xlabel "Usines"
set ylabel "Volume"

set style data histograms
set style fill solid 0.8
set boxwidth 0.6

set grid ytics
set xtics rotate by -90 font ",9"

plot "data.dat" using 2:xtic(1) notitle
EOF


tail -n +2 "$donnee" | sort -t';' -k2,2n > triees.dat
  {
    head -n 1 "$donnee"
    head -n 50 triees.dat
  } > histo.txt
  rm triees.dat
  awk -F';' 'NR>1 {print "\"" $1 "\"",$2}' histo.txt > data.dat
  # Générer le bar chart
  gnuplot << EOF
set terminal png size 1000,600
set output "histo_plus_petites.png"

set title "Volume par usine ($valeur)"
set xlabel "Usines"
set ylabel "Volume"

set style data histograms
set style fill solid 0.8
set boxwidth 0.6

set grid ytics
set xtics rotate by -90 font ",9"

plot "data.dat" using 2:xtic(1) notitle
EOF
  echo "Diagramme généré"
  

elif [[ "$action" == "leaks" ]]; then
  if [[ -z "$valeur" ]]; then
    echo "Erreur: leaks nécessite un 2ème paramètre (ex: \"Facility complex #RH400057F\")"
    exit 1
  fi

  ./principal "leaks" "$valeur"

else
  echo "Erreur: action invalide '$action' (attendu: hist ou leaks)"
  exit 1
fi

end=$(date +%s)
echo "Temps d'exécution : $((end - start)) secondes"
