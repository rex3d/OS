# Najlepszy rozbudowany Makefile dla katalogu kernel/core

# Nazwa pliku wynikowego
TARGET := kernel_core

# Kompilator i linker
CC      := gcc
LD      := ld
AS      := nasm

# Flagi kompilatora
CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. 
LDFLAGS := -T linker.ld -nostdlib

# Pliki źródłowe i obiektowe
SRCS    := $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)

# Opcjonalnie: pliki assemblera
ASMS    := $(wildcard *.asm)
ASMOBJS := $(ASMS:.asm=.o)

# Domyślna reguła
all: $(TARGET).bin

# Kompilacja plików C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Kompilacja plików assemblera (jeśli są)
%.o: %.asm
	$(AS) -f elf32 $< -o $@

# Linkowanie
$(TARGET).bin: $(OBJS) $(ASMOBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Reguła do uruchamiania QEMU (jeśli kernel jest bootowalny)
run: $(TARGET).bin
	qemu-system-i386 -kernel $(TARGET).bin

# Reguła do debugowania z GDB
debug: $(TARGET).bin
	qemu-system-i386 -kernel $(TARGET).bin -s -S

# Czyszczenie plików pośrednich i wynikowych
clean:
	rm -f *.o *.bin

# Automatyczne zależności
-include $(SRCS:.c=.d)

# Generowanie plików zależności
%.d: %.c
	@set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o \1.d : /g' > $@; [ -s $@ ] || rm -f $@

.PHONY: all clean run debug