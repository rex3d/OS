#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Typy systemów plików
typedef enum {
    FS_TYPE_UNKNOWN = 0,
    FS_TYPE_FAT12,
    FS_TYPE_FAT16,
    FS_TYPE_FAT32,
    FS_TYPE_TMPFS,
    FS_TYPE_DEVFS,
    FS_TYPE_COUNT
} fs_type_t;

// Maksymalna długość punktu montowania (w tym znak null)
#define FS_MOUNT_POINT_MAX 64

// Deskryptor zamontowanego systemu plików
typedef struct filesystem {
    fs_type_t type;
    void* fs_data; // Wskaźnik na strukturę FAT12/FAT16/FAT32/tmpfs/devfs
    char mount_point[FS_MOUNT_POINT_MAX];
    struct filesystem* next;
} filesystem_t;

// Rejestracja i montowanie systemu plików
int filesystem_mount(fs_type_t type, const char* mount_point, void* fs_data);
int filesystem_unmount(const char* mount_point);

// Wyszukiwanie systemu plików po ścieżce
const filesystem_t* filesystem_find(const char* path);

// Lista zamontowanych systemów plików
void filesystem_list(void (*cb)(const filesystem_t* fs, void* user_data), void* user_data);

// Zwraca liczbę zamontowanych systemów plików
size_t filesystem_count(void);

#ifdef __cplusplus
}
#endif

#endif // FILESYSTEM_H