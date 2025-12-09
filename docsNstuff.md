# Ressources

## Optimization for data handling

## Basic commands
```sh
sed 's/"your pattern"/"by what you replace"/"you can put g to make it general"' <file>
# it outputs like an echo so you can use > to place the output in a file
by the way when you want to modify it all you can use a temporary file exemple :
touch tmp_ && sed 's/#/;#/g' myfile.dat >tmp_ && mv tmp_  myfile.dat
```
## histogram
