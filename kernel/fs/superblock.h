#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Struktura opisująca superblok systemu plików
struct superblock {
    const char *name;                     // Nazwa systemu plików (np. "ext2", "fat32")
    uint32_t fs_type;                     // Unikalny ID systemu plików
    uint64_t block_size;                  // Rozmiar bloku w bajtach
    uint64_t total_blocks;                // Całkowita liczba bloków
    uint64_t free_blocks;                 // Dostępna liczba bloków

    void *private_data;                   // Wskaźnik na dane specyficzne dla danego FS

    // Callbacki systemu plików
    int (*mount)(struct superblock *sb, void *device);   // Montowanie systemu plików
    int (*unmount)(struct superblock *sb);               // Odmontowanie systemu plików
};

// Rejestruje superblok do globalnej tablicy
int register_superblock(struct superblock *sb);

// Wyszukuje superblok po nazwie systemu plików
struct superblock *get_superblock_by_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif // SUPERBLOCK_H
