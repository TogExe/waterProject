# waterProject
This program is our school project about treating data.

##installation : 
---
To setup our program simply use this command in your console :
```
git clone https://github.com/TogExe/waterProject
cd waterProject
chmod +x script/wildwaterlauncher.sh
```
to add `wildwaterlauncher` to your commands 
```sh
sudo ln -s ~script/wildwaterlauncher.sh /usr/local/bin/wildwaterlauncher
```
and to remove it 
```sh
sudo rm /usr/local/bin/wildwaterlauncher
```
to see the commands run it with no parameter
```sh
./script/wildwaterlauncher.sh
```

Now everything should be set so i'll present you how to use our program.

Firstly, you can have access to the different commands by typing :
```sh
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
As you have four options:
`--debug` to switch to debug mode
`--fast`to make the program run as fast as possible without worrying about security *(outdated)*
`--rsc` to use a new cache
`--make` to compile the program

You also have two commands in this program, "histo" and "leaks". 
The first one will allow you to create an histogram of the factory in function of which parameter you choose.
The command works like this :

```sh
./script/wildwaterlauncher.sh [DATAFILENAME] histo [max / src / real]
```
(without the [], it was just to show the things you had to modify depending on what you want and the name of the datafile)

The differents parameters are : 
-max, which is the maximum capacity of the factories in M.m^3.
-src, which is the volume the factories can receive from all their sources
-real, which is the real volume they'll perceive from the sources. (Volume - Leaks)

After using this command, the program will create a new datafile with all the factories in the first column (sorted in reverse alphabetical order by ID), the maximum capacity in the second column,
and the data you chose in the parameters (if you chose max, this column will be empty since the data you needed is in the second column).
The program will also create an histogram. The first one will be the data of the 50 smallest factories (the 50 factories with the lowest maximum capacities) and the second one will be an histogram of the 10 biggest (the 10 factories with the highest maximum capacities).


The second command is "leaks". This command will display the amount of water lost from the factory you choose. However, if the factory you used as parameter does not exist then it will display "-1".
You can use this command like this : 

```
./script/wildwaterlauncher.sh [DATAFILENAME] leaks "NAME #ID"
```
("" included, [] not included)
It will also create/update if the file exist, a datafile containing the calculated yield history.

![da image](https://github.com/TogExe/waterProject/blob/main/waterrr.webp)
