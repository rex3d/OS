# Główny Makefile kernela z obsługą brakujących katalogów/plików

SUBDIRS := core arch/x86/apic arch/x86/asm arch/x86/smp

.PHONY: all clean run debug $(SUBDIRS)

all:
	@for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ]; then \
			$(MAKE) -C $$dir; \
		fi \
	done
	@echo "Linkowanie kernela..."
	@OBJS=""; \
	for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ]; then \
			for obj in $$dir/*.o; do \
				[ -e "$$obj" ] && OBJS="$$OBJS $$obj"; \
			done; \
		fi \
	done; \
	ld -T kernel.ld -nostdlib -o kernel.bin $$OBJS

clean:
	@for dir in $(SUBDIRS); do \
		if [ -d "$$dir" ]; then \
			$(MAKE) -C $$dir clean; \
		fi \
	done
	rm -f kernel.bin

run: all
	qemu-system-i386 -kernel kernel.bin

debug: all
	qemu-system-i386 -kernel kernel.bin -s -S