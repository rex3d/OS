#ifndef READ_SECTOR_H
#define READ_SECTOR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Typ funkcji odczytu sektora:
//  - lba: numer sektora (Logical Block Addressing)
//  - buf: wskaźnik do bufora na dane sektora (512 bajtów)
// Zwraca 0 przy sukcesie, inna wartość przy błędzie.
typedef int (*read_sector_fn)(uint32_t lba, void* buf);

// Domyślna implementacja funkcji odczytu sektora
int read_sector(uint32_t lba, void* buf);

// Ustawia własną implementację odczytu sektora.
// Przypisuje funkcję, która zostanie wywołana przez read_sector.
void read_sector_set_impl(read_sector_fn fn);

#ifdef __cplusplus
}
#endif

#endif // READ_SECTOR_H
