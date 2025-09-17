## 4. Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include
LDFLAGS = -lm

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

EXAMPLES = memory_example tickstack_example higgs_example mirror_example wisdom_example

.PHONY: all examples clean

all: examples

examples: $(EXAMPLES)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

memory_example: $(SRC_DIR)/examples/memory_example.c $(INCLUDE_DIR)/fixed_memory_systems.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

tickstack_example: $(SRC_DIR)/examples/tickstack_example.c $(INCLUDE_DIR)/fixed_tickstack_11d.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

higgs_example: $(SRC_DIR)/examples/higgs_example.c $(INCLUDE_DIR)/higgs_king.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

mirror_example: $(SRC_DIR)/examples/mirror_example.c $(INCLUDE_DIR)/mirror_king.h $(INCLUDE_DIR)/junk_warrior.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

wisdom_example: $(SRC_DIR)/examples/wisdom_example.c $(INCLUDE_DIR)/O_wisdom.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
