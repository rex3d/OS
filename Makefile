all: boot kernel iso

boot:
	$(MAKE) -C boot

kernel:
	$(MAKE) -C kernel

iso:
	mkdir -p iso/boot/grub
	cp boot/grub/grub.cfg iso/boot/grub/
	cp boot/bootloader.bin iso/boot/
	cp kernel/kernel.bin iso/boot/
	grub-mkrescue -o RexOS.iso iso

run:
	qemu-system-x86_64 -cdrom RexOS.iso
