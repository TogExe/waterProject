# Ressources

## Optimization for data handling

## Basic commands
```sh
sed 's/"your pattern"/"by what you replace"/"you can put g to make it general"' <file>
# it outputs like an echo so you can use > to place the output in a file
#by the way when you want to modify it all you can use a temporary file exemple :
touch tmp_ && sed 's/#/;#/g' myfile.dat >tmp_ && mv tmp_  myfile.dat

# commands are also faster than c for handling files so we must offload as many tasks from the c program to the bash program as possible
# Calculations and graphs stay faster in c so to handle AVL we shall use C

```
## histogram
