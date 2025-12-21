CC = gcc
# Common base flags
BASE_CFLAGS =  -Wall -Wextra -g -O3

# Optimization flags for the Main build
# Added -flto here so the objects are prepared for Link-Time Optimization
CFLAGS = $(BASE_CFLAGS) -Ofast -flto -march=native

# Optimization flags for the Sorter
# Removed -fprofile-use for the default build to avoid errors
FAST_FLAGS = -march=native -flto -Wall

# Linking flags - IMPORTANT: flto must be here too!
LDFLAGS = -flto -Ofast -march=native

# Directories
SRC_DIR = src
BUILD_DIR = build

TARGET_MAIN = $(BUILD_DIR)/linkingpark
TARGET_SORT = $(BUILD_DIR)/ultra_sort

SRC_MAIN = $(wildcard $(SRC_DIR)/*.c)
OBJ_MAIN = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_MAIN))
SRC_SORT = speed/ultra_sort_with_ui.c

all: $(TARGET_MAIN) $(TARGET_SORT)

# Link Main Executable
$(TARGET_MAIN): $(OBJ_MAIN)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ -lm
	@echo "Built $(TARGET_MAIN) with LTO"

# Build Ultra Sorter
$(TARGET_SORT): $(SRC_SORT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FAST_FLAGS) $(LDFLAGS) $< -o $@
	@echo "Built $(TARGET_SORT) with max optimizations"

# Compile .c -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) *.gcda *.gcno
