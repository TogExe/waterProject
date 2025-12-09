# Ressources

## Data handling ideas

implementing multiple threads to handle multiple branchese at once 

exemple 
```
top (run with main process)
 -> rb 
     -> rb (handled with Thread 1)
     -> lb (handled with Thread 2)
 -> lb
     -> rb (handled with Thread 3)
     -> lb (handled with Thread 4)
```
since my MultiThreading system is close to done it could be safe and be used for dividing the data handling time by the number of possible threads considering that we will have an equilibrated tree this could accelerate the speed.

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
