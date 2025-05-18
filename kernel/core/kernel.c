// kernel.c - uniwersalny start dla 32/64-bitowego kernela

#if defined(__x86_64__) || defined(_M_X64)
    #define KERNEL_64BIT
#endif

// Proste makro do logowania (możesz podmienić na własną funkcję)
#ifdef KERNEL_64BIT
    // Wersja 64-bitowa: logowanie można rozbudować o obsługę portu szeregowego lub framebuffer
    #define KERNEL_LOG(msg) /* TODO: Dodaj obsługę logowania na VGA/serial */
#else
    #include "video.h"
    #define KERNEL_LOG(msg) video_print(msg)
#endif

#ifdef KERNEL_64BIT
// 64-bitowy kernel
void kernel_main(void) {
    KERNEL_LOG("Kernel 64-bit startuje!\n");

    // TODO: Inicjalizacja sprzętu, obsługa błędów, zarządzanie pamięcią, itp.

    while (1) {
        __asm__ volatile ("hlt");
    }
}
#else
// 32-bitowy kernel
#include "keyboard.h"
#include "shell.h"
#include "loader_screen.h"

void kernel_main(void) {
    video_clear_screen();
    show_loading_screen();  // Wyświetl ekran ładowania

    KERNEL_LOG("Kernel 32-bit startuje! Wersja 0.2.\n");

    if (!keyboard_init()) {
        KERNEL_LOG("Błąd: Nie udało się zainicjalizować klawiatury!\n");
        while (1) { __asm__ volatile ("hlt"); }
    }

    shell_start();  // Inicjalizacja powłoki
    shell_run();    // Uruchomienie powłoki

    while (1) {
        __asm__ volatile ("hlt");
    }
}
#endif