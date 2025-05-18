#include "elf_loader.h"
#include <stdint.h>
#include <string.h>

#define EI_NIDENT 16
#define PT_LOAD   1
#define ELF_MAGIC0 0x7F
#define ELF_MAGIC1 'E'
#define ELF_MAGIC2 'L'
#define ELF_MAGIC3 'F'

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

/**
 * Ładuje ELF-a do pamięci.
 * @param elf_data Wskaźnik na bufor zawierający dane ELF-a.
 * @return Adres entry pointa lub -1 jeśli coś poszło nie tak.
 */
int elf32_load(const void* elf_data) {
    if (!elf_data) return -1;

    const Elf32_Ehdr* ehdr = (const Elf32_Ehdr*)elf_data;

    // Sprawdzenie magicznych bajtów
    if (ehdr->e_ident[0] != ELF_MAGIC0 || ehdr->e_ident[1] != ELF_MAGIC1 ||
        ehdr->e_ident[2] != ELF_MAGIC2 || ehdr->e_ident[3] != ELF_MAGIC3) {
        return -1; // Nieprawidłowy ELF
    }

    const Elf32_Phdr* phdr = (const Elf32_Phdr*)((const uint8_t*)elf_data + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) continue;

        // Kopiowanie segmentu do pamięci
        memcpy((void*)phdr[i].p_vaddr,
               (const uint8_t*)elf_data + phdr[i].p_offset,
               phdr[i].p_filesz);

        // Zerowanie reszty jeśli segment w pamięci ma być większy
        if (phdr[i].p_memsz > phdr[i].p_filesz) {
            memset((void*)(phdr[i].p_vaddr + phdr[i].p_filesz),
                   0,
                   phdr[i].p_memsz - phdr[i].p_filesz);
        }
    }

    return ehdr->e_entry;
}
