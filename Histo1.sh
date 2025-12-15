#!/bin/bash

input="c-wildwater_v0.dat"   
output="result.csv"   

# On réinitialise le fichier résultat
: > "$output"

echo "Nom ; ID ; Capacite traitement en milliers de m^3" >> $output

while IFS=';' read -r c1 c2 c3 c4 c5
do
    if [[ "$c5" = "-" ]]; then
        echo "$c2;$c4" >> "$output"
    fi

done < "$input"

touch tmp.csv && sed 's/#/; #/g' result.csv > tmp.csv && mv tmp.csv  result.csv





