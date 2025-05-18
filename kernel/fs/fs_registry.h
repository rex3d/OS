#ifndef FS_REGISTRY_H
#define FS_REGISTRY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Typ funkcji montującej system plików
typedef int (*fs_mount_fn)(void* params, void** fs_data);

// Maksymalna długość nazwy systemu plików
#define FS_REGISTRY_MAX_NAME_LEN 64

// Rejestracja typu systemu plików
// Zwraca 0 przy sukcesie, <0 przy błędzie (np. duplikat, brak pamięci)
int fs_registry_register(const char* fs_name, fs_mount_fn mount_fn);

// Wyszukiwanie funkcji montującej po nazwie systemu plików
// Zwraca wskaźnik na funkcję lub NULL jeśli nie znaleziono
fs_mount_fn fs_registry_find(const char* fs_name);

// Iteracja po wszystkich zarejestrowanych typach systemów plików
// cb - funkcja wywoływana dla każdego wpisu
void fs_registry_list(void (*cb)(const char* fs_name, fs_mount_fn fn, void* user_data), void* user_data);

// Usuwa wszystkie zarejestrowane systemy plików (opcjonalnie, do czyszczenia zasobów)
void fs_registry_clear(void);

#ifdef __cplusplus
}
#endif

#endif // FS_REGISTRY_H