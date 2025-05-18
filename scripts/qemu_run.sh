#!/bin/bash
set -e

# Tworzymy obraz dysku 10MB
dd if=/dev/zero of=disk.img bs=512 count=20480

# Kopiujemy bootloader (512 bajtów) na początek dysku
dd if=bootloader/bootloader.bin of=disk.img bs=512 count=1 conv=notrunc

# Kopiujemy kernel na dysk (od sektora 2 - offset 1KB, czyli 2*512B)
dd if=kernel/kernel.bin of=disk.img bs=512 seek=2 conv=notrunc

# Uruchamiamy QEMU
qemu-system-i386 -drive format=raw,file=disk.img -m 512M -serial stdio

# Uruchomienie 64-bitowego kernela (jeśli chcesz testować long mode)
qemu-system-x86_64 -drive format=raw,file=disk.img -m 512M -serial stdio

# -smp 4
# -smp 4 -m 512M
# -smp 4 -m 512M -kernel kernel/kernel.bin
# -kernel kernel/kernel.bin
# -kernel kernel/kernel.bin -smp 4 -m 512M
# -kernel kernel/kernel.bin -smp 4 -m 512M -serial stdio
# -kernel kernel/kernel.bin -smp 4 -m 512M -serial stdio -nographic
# -kernel kernel/kernel.bin -smp 4 -m 512M -serial stdio -nographic
# -kernel kernel/kernel.bin -smp 4 -m 512M -serial stdio -nographic -hda disk.img