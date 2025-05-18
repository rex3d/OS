#!/bin/bash
set -e

# Kompilacja C
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/kernel.c -o kernel/kernel_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/video.c -o kernel/video_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/keyboard.c -o kernel/keyboard_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/shell.c -o kernel/shell_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/interrupts.c -o kernel/interrupts_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/paging.c -o kernel/paging_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/multitasking.c -o kernel/multitasking_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/filesystem.c -o kernel/filesystem_c.o
i686-elf-gcc -m32 -ffreestanding -O2 -c kernel/utils.c -o kernel/utils_c.o

# Kompilacja asm (z innymi nazwami .o, żeby się nie nadpisywały)
nasm -f elf32 kernel/boot.asm -o kernel/boot.o
nasm -f elf32 kernel/interrupts.asm -o kernel/interrupts_asm.o
nasm -f elf32 kernel/paging.asm -o kernel/paging_asm.o
nasm -f elf32 kernel/multitasking.asm -o kernel/multitasking_asm.o
nasm -f elf32 kernel/filesystem.asm -o kernel/filesystem_asm.o
nasm -f elf32 kernel/utils.asm -o kernel/utils_asm.o
nasm -f elf32 kernel/porty.asm -o kernel/porty.o

# Linkowanie (podaj tylko raz każdy plik .o, nie mieszaj z tymi samymi nazwami)
i686-elf-ld -T kernel/kernel.ld -o kernel/kernel.bin \
    kernel/boot.o \
    kernel/kernel_c.o kernel/video_c.o kernel/keyboard_c.o kernel/shell_c.o kernel/porty.o \
    kernel/interrupts_c.o interrupts_asm.o \
    kernel/paging_c.o paging_asm.o \
    kernel/multitasking_c.o multitasking_asm.o \
    kernel/filesystem_c.o filesystem_asm.o \
    kernel/utils_c.o utils_asm.o

echo "Kernel skompilowany do kernel/kernel.bin"

# Przygotowanie ISO
mkdir -p iso/boot/grub
cp kernel/kernel.bin iso/boot/kernel.bin

# Grub config (upewnij się, że masz plik grub.cfg w kernel/)
cp kernel/grub.cfg iso/boot/grub/grub.cfg

grub-mkrescue -o os.iso iso

echo "Obraz ISO stworzony jako os.iso"

# Nie usuwaj plików potrzebnych do debugowania czy zmian!
# rm -rf kernel/*.o iso/boot


gcc_files+=("kernel/shell/shell.c")