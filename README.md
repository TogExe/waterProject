# WaterProject

This program is a school project for processing and analyzing water data.

## Installation

Clone the repository and make the launcher executable:

```bash
git clone https://github.com/TogExe/waterProject
cd waterProject
chmod +x script/wildwaterlauncher.sh
```

### Add the launcher to your commands

```bash
sudo ln -s ~/waterProject/script/wildwaterlauncher.sh /usr/local/bin/wildwaterlauncher
```

### Remove the launcher

```bash
sudo rm /usr/local/bin/wildwaterlauncher
```

### Run the launcher

```bash
./script/wildwaterlauncher.sh
```

---

## Usage

Running the launcher without arguments will display available commands and options.

Available options:

* `--debug` : Enable debug mode
* `--fast` : Run faster without security checks *(deprecated)*
* `--rsc` : Use a new cache
* `--make` : Compile the program

The program provides two main commands: **histo** and **leaks**.

---

### 1. Histogram (`histo`)

Create a histogram of factory data:

```bash
./script/wildwaterlauncher.sh [DATAFILE] histo [max|src|real]
```

* `max` : Maximum capacity of factories (M.m³)
* `src` : Volume factories can receive from sources
* `real` : Actual volume received (Volume minus leaks)

Output:

* A data file with factory IDs and selected data
* Two histograms:

  * 50 smallest factories (by maximum capacity)
  * 10 largest factories

---

### 2. Leaks (`leaks`)

Display water lost from a specific factory:

```bash
./script/wildwaterlauncher.sh [DATAFILE] leaks "NAME #ID"
```

* If the factory does not exist, it returns `-1`.
* Updates or creates a file with yield history.

---

### Example

```bash
./script/wildwaterlauncher.sh data.csv histo real
./script/wildwaterlauncher.sh data.csv leaks "Factory #123"
```


If you want, I can also **reformat the README to include all images nicely with captions** for a cleaner visual presentation. Do you want me to do that?
