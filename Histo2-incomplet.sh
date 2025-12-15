#!/bin/bash

input="c-wildwater_v0.dat"   
output="result2.csv"   


: > "$output"

echo "Factory Name; Factory ID; volume capte d'une source; Capacite traitement en milliers de m^3" >> $output

while IFS=';' read -r c1 c2 c3 c4 c5
do
    if [ "$c1" = "-" ] && [ "$c3" != "-" ] && [ "$c4" != "-" ]; then
        echo "$c3;$c4" >> "$output"
    fi
done < "$input"

touch tmp.csv && sed 's/#/; #/g' result2.csv > tmp.csv && mv tmp.csv  result2.csv


input="result2.csv"
output="tmp.csv"

while IFS=";" read -r c1 c2 c3
do 
    if  