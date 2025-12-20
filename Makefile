# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TARGET = $(BUILD_DIR)/linkingpark

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@echo "Built $(TARGET)"

# Compile .c -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"

# Phony targets
.PHONY: all clean
