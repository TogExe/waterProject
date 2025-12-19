# Simple Makefile for linkingpark project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRC = src/main.c src/customgraphstruct.c src/plantline.c src/mmap_reader.c 
OBJ = $(SRC:.c=.o)
TARGET = linkingpark

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
