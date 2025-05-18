# Ścieżki
ASM_SRC = asm/bootloader.asm
BUILD_DIR = build
BOOT_BIN = $(BUILD_DIR)/bootloader.bin
ISO_DIR = iso
ISO_BOOT = $(ISO_DIR)/boot
ISO_IMAGE = $(BUILD_DIR)/os.iso

# Komendy
NASM = nasm
GRUB_MKRESCUE = grub-mkrescue

# Cele
all: $(BOOT_BIN) iso

$(BOOT_BIN): $(ASM_SRC)
	@mkdir -p $(BUILD_DIR)
	$(NASM) -f bin -o $(BOOT_BIN) $(ASM_SRC)

iso: $(BOOT_BIN)
	@mkdir -p $(ISO_BOOT)
	cp $(BOOT_BIN) $(ISO_BOOT)/bootloader.bin
	@mkdir -p $(ISO_DIR)/boot/grub
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "RexOS" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  multiboot /boot/bootloader.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO_IMAGE) $(ISO_DIR)

run: all
	qemu-system-i386 -cdrom $(ISO_IMAGE)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)
