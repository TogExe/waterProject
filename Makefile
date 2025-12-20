# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
# Performance flags for the sorter
FAST_FLAGS = -Ofast -march=native -Wall

# Directories
SRC_DIR = src
BUILD_DIR = build

# Targets
TARGET_MAIN = $(BUILD_DIR)/linkingpark
TARGET_SORT = $(BUILD_DIR)/ultra_sort

# Source files for Main
SRC_MAIN = $(wildcard $(SRC_DIR)/*.c)
OBJ_MAIN = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_MAIN))

# Source files for Sorter (Assuming it's in the root or src)
# Change 'ultra_sort.c' to '$(SRC_DIR)/ultra_sort.c' if you move it there
SRC_SORT = speed/ultra_sort_with_ui.c

# Default target
all: $(TARGET_MAIN) $(TARGET_SORT)

# Build Main Executable
$(TARGET_MAIN): $(OBJ_MAIN)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@echo "Built $(TARGET_MAIN)"

# Build Ultra Sorter Executable (Standalone compilation)
$(TARGET_SORT): $(SRC_SORT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FAST_FLAGS) $< -o $@
	@echo "Built $(TARGET_SORT) with -Ofast"

# Compile .c -> .o for Main
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"

.PHONY: all clean
