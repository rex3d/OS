ENTRY(_start)

SECTIONS
{
    /* Adres początkowy kernela w pamięci fizycznej */
    . = 0x00100000;

    /* Sekcja kodu */
    .text : ALIGN(4K) {
        *(.multiboot)      /* Multiboot header jeśli używasz */
        *(.text*)
    }

    /* Sekcja danych tylko do odczytu */
    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    /* Sekcja danych inicjalizowanych */
    .data : ALIGN(4K) {
        *(.data*)
    }

    /* Sekcja danych niezainicjalizowanych (BSS) */
    .bss : ALIGN(4K) {
        *(.bss*)
        *(COMMON)
    }

    /* Stos (opcjonalnie, jeśli chcesz zarezerwować miejsce na stos) */
    .stack : ALIGN(4K) {
        . = . + 0x8000; /* 32 KB stack */
        stack_top = .;
    }

    /* Symbol końca kernela */
    end = .;
}