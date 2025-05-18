# === Konfiguracja ===
CC = gcc
CFLAGS = -Wall -Wextra -O2 -ffreestanding -fno-stack-protector -nostdlib -fno-builtin -m32
AR = ar
ARFLAGS = rcs

SRC_DIR := .
OBJ_DIR := build
TARGET := fs.a

# znajdź wszystkie pliki .c
SRCS := $(wildcard $(SRC_DIR)/*.c)

# zamień *.c → build/*.o
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# === Reguły ===

all: $(TARGET)

# tworzenie biblioteki statycznej
$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# kompilacja .c → .o do katalogu build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) *.a

.PHONY: all clean
