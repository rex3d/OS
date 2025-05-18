#include "fs_utils.h"
#include <string.h>
#include <stddef.h>

// Pomija wszystkie początkowe ukośniki
const char* fs_path_skip_root(const char* path) {
    if (!path) return NULL;
    while (*path == '/') ++path;
    return path;
}

// Normalizuje ścieżkę: usuwa powtarzające się ukośniki
void fs_path_normalize(const char* src, char* dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0) return;
    size_t j = 0;
    int prev_slash = 0;
    for (size_t i = 0; src[i] && j + 1 < dst_size; ++i) {
        char c = src[i];
        if (c == '/') {
            if (prev_slash) continue;
            prev_slash = 1;
        } else {
            prev_slash = 0;
        }
        dst[j++] = c;
    }
    // Usuń końcowy ukośnik (jeśli nie jest to root '/')
    if (j > 1 && dst[j-1] == '/') --j;
    dst[j] = '\0';
}

// Rozdziela ścieżkę na katalog i nazwę pliku
void fs_path_split(const char* path, char* dir, size_t dir_size, char* name, size_t name_size) {
    if (!path || !dir || !name || dir_size == 0 || name_size == 0) return;
    const char* last = strrchr(path, '/');
    if (last) {
        size_t dlen = (size_t)(last - path);
        if (dlen >= dir_size) dlen = dir_size - 1;
        memcpy(dir, path, dlen);
        dir[dlen] = '\0';
        strncpy(name, last + 1, name_size - 1);
        name[name_size - 1] = '\0';
    } else {
        dir[0] = '\0';
        strncpy(name, path, name_size - 1);
        name[name_size - 1] = '\0';
    }
}