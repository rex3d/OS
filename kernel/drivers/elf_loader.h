#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Ładuje 32-bitowego ELF-a do pamięci.
 * 
 * @param elf_data  Wskaźnik na dane pliku ELF w pamięci.
 * @return          Adres entry pointa lub -1 jeśli plik jest nieprawidłowy.
 */
int elf32_load(const void* elf_data);

#ifdef __cplusplus
}
#endif