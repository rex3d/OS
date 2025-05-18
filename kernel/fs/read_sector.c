#include "read_sector.h"
#include <string.h>

// Aktualna funkcja do odczytu sektora (ustawiana dynamicznie)
static read_sector_fn current_read_sector = NULL;

// Ustawia funkcję odczytu sektora
void read_sector_set_impl(read_sector_fn fn) {
    current_read_sector = fn;
}

// Odczytuje sektor z dysku (LBA) do bufora
// Zwraca 0 przy sukcesie, <0 przy błędzie
int read_sector(uint32_t lba, void* buf) {
    if (current_read_sector)
        return current_read_sector(lba, buf);
    // Brak ustawionej implementacji - czyścimy bufor i zwracamy błąd
    if (buf)
        memset(buf, 0, 512);
    return -1;
}