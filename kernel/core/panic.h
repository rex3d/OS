#ifndef PANIC_H
#define PANIC_H

#include <stdnoreturn.h>
#include <stdbool.h>
#include <stdint.h>

// Opcjonalnie: Makro do uzyskania informacji o pliku i linii
#define PANIC(msg) panic_full((msg), __FILE__, __LINE__, __func__)

// Struktura przechowująca szczegóły panic
typedef struct {
    const char* message;
    const char* file;
    int line;
    const char* function;
    uint64_t timestamp;
    uint32_t cpu_id;
    uint32_t error_code;
} panic_info_t;

// Główna funkcja panic - minimalna wersja
noreturn void panic(const char* message);

// Rozszerzona funkcja panic z dodatkowymi informacjami
noreturn void panic_full(const char* message, const char* file, int line, const char* function);

// Funkcja do rejestrowania panic (np. do logów lub portu szeregowego)
void panic_log(const panic_info_t* info);

// Funkcja do ustawiania kodu błędu panic
void panic_set_error_code(uint32_t code);

// Funkcja do pobierania ostatnich informacji o panic
const panic_info_t* panic_get_last_info(void);

// Funkcja do sprawdzenia, czy system jest w stanie panic
bool panic_is_active(void);

// Funkcja do inicjalizacji obsługi panic (np. ustawienie handlerów)
void panic_init(void);

// Funkcja do wymuszenia dumpu stosu podczas panic
void panic_dump_stack(void);

// Funkcja do wymuszenia zrzutu pamięci podczas panic
void panic_dump_memory(void);

// Funkcja do resetowania systemu po panic (jeśli obsługiwane)
noreturn void panic_system_reset(void);

#endif // PANIC_H