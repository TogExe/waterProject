# waterProject
This program is our school project about treating data.

To download our program you can simply use this command in your console :
```
git clone https://github.com/TogExe/waterProject
cd waterProject
```

To compile it, you can simply use the command :
```
make
```

Now everything should be set so i'll present you how to use our program.

Firstly, you can have access to the different commands by typing :
```
./script/wildwaterlauncher.sh
```
```
 ╭──────────────────────────────────────────────────────────────────╮
 │                   ◖ WildWater Shell Launcher ◗                   │
 │  Usage: ./script/wildwaterlauncher.sh DATAFILE COMMAND [PARAMS]  │
 ├──────────────────────────────────────────────────────────────────┤
 │                                                                  │
 │ ◖ OPTIONS ◗                                                      │
 │    --debug      Verbose trace                                    │
 │    --fast       Max speed                                        │
 │    --rsc        Reset cache                                      │
 │    --make       Compile program                                  │
 │                                                                  │
 │ ◖ COMMANDS ◗                                                     │
 │    histo TYPE   Generates histogram                              │
 │    - options: max, src real                                      │
 │    leaks ID      Computes leaks                                  │
 ╰──────────────────────────────────────────────────────────────────╯
  Example: ./script/wildwaterlauncher.sh data.dat leaks "Unit #WJ100255G"
  Example: ./script/wildwaterlauncher.sh data.dat histo src
```

As you can see, you have two commands in this program, "histo" and "leaks". 
The first one will allow you to create two histograms of the factory in function of which parameter you choose.
The command works like this :

```
./script/wildwaterlauncher.sh [DATAFILENAME] histo [max / vol / real]
```
(without the [], it was just to show the things you had to modify depending on what you want and the name of the datafile)

The differents parameters are : 
-Max, which is the maximum capacity of the factories in M.m^3.
-Vol, which is the volume the factories can receive from all their sources
-Real, which is the real volume they'll perceive from the sources. (Volume - Leaks)

After using this command, the program will create a new datafile with all the factories in the first column (sorted alphabetically), the maximum capacity in the second column,
and the data you chose in the parameters (if you chose "Max", this column will be empty since the data you needed is in the second column).
The program will also create two histograms. The first one will be the data of the 50 smallest factories (the 50 factories with the lowest maximum capacities) and the second one will be an histogram of the 10 biggest (the 10 factories with the highest maximum capacity).


The second command is "leaks". This command will display the amount of water lost from the factory you choose. However, if the factory you used as parameter does not exist then it will display "-1".
You can use this command like this : 

```
./wildwaterlauncher.sh [DATAFILENAME] leaks "NAME #ID"
```
("" included, [] not included)
It will also create/update if the file exist, a datafile containing the calculated yield history.

![da image](https://github.com/TogExe/waterProject/blob/main/waterrr.webp)
