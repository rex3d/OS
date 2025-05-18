#include "init.h"

#if !defined(__x86_64__) && !defined(_M_X64)
#include "video.h"
#include "keyboard.h"
#include "timer.h"
#include "memory.h"
#include "interrupts.h"
#include "devices.h"
#include "filesystem.h"
#include "process.h"
#endif

void kernel_init(void) {
#if !defined(__x86_64__) && !defined(_M_X64)
    // Inicjalizacja ekranu
    video_clear_screen();
    video_print("Kernel initializing...\n");

    // Inicjalizacja klawiatury
    keyboard_init();
    video_print("Keyboard initialized.\n");

    // Inicjalizacja timera
    timer_init();
    video_print("Timer initialized.\n");

    // Inicjalizacja pamięci
    memory_init();
    video_print("Memory initialized.\n");

    // Inicjalizacja obsługi przerwań
    interrupts_init();
    video_print("Interrupts initialized.\n");

    // Inicjalizacja urządzeń
    devices_init();
    video_print("Devices initialized.\n");

    // Inicjalizacja systemu plików
    filesystem_init();
    video_print("Filesystem initialized.\n");

    // Inicjalizacja zarządzania procesami
    process_init();
    video_print("Process management initialized.\n");

    // Dodaj kolejne inicjalizacje w razie potrzeby

    video_print("Kernel initialization complete.\n");
#else
    // Wersja 64-bitowa: dodaj własne inicjalizacje tutaj
    // Przykład:
    // init_apic();
    // init_paging();
    // init_syscalls();
    // init_smp();
    // Dodaj kolejne inicjalizacje specyficzne dla 64-bitowego jądra w tym miejscu
#endif
}