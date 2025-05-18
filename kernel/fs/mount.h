#ifndef MOUNT_H
#define MOUNT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

// Struktura punktu montowania
typedef struct mount_point {
    char path[64];           // Punkt montowania (ścieżka)
    filesystem_t* fs;        // Wskaźnik na system plików
    struct mount_point* next; // Następny w liście
} mount_point_t;

// Montuje system plików pod daną ścieżką
// Zwraca 0 przy sukcesie, <0 przy błędzie
int mount_fs(const char* path, fs_type_t type, void* fs_data);

// Odmontowuje system plików pod daną ścieżką
// Zwraca 0 przy sukcesie, <0 jeśli nie znaleziono
int umount_fs(const char* path);

// Znajduje punkt montowania najbardziej pasujący do podanej ścieżki
mount_point_t* mount_find(const char* path);

// Iteruje po wszystkich punktach montowania, wywołując callback
void mount_list(void (*cb)(const mount_point_t*, void*), void* user_data);

#ifdef __cplusplus
}
#endif

#endif // MOUNT_H
