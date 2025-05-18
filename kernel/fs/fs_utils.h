#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns true if the path is absolute (starts with '/')
static inline bool fs_path_is_absolute(const char* path) {
    return path && path[0] == '/';
}

// Skips leading '/' characters in the path (returns pointer after root slashes)
static inline const char* fs_path_skip_root(const char* path) {
    if (!path) return NULL;
    while (*path == '/') ++path;
    return path;
}

// Copies and normalizes the path (removes duplicate '/')
// dst is always null-terminated if dst_size > 0
static inline void fs_path_normalize(const char* src, char* dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0) return;
    size_t di = 0;
    bool prev_slash = false;
    for (size_t si = 0; src[si] && di + 1 < dst_size; ++si) {
        if (src[si] == '/') {
            if (!prev_slash) {
                dst[di++] = '/';
                prev_slash = true;
            }
        } else {
            dst[di++] = src[si];
            prev_slash = false;
        }
    }
    // Remove trailing slash (except root '/')
    if (di > 1 && dst[di-1] == '/') --di;
    dst[di] = '\0';
}

// Splits path into directory and filename
// dir and name are always null-terminated if their sizes > 0
static inline void fs_path_split(const char* path, char* dir, size_t dir_size, char* name, size_t name_size) {
    if (!path) {
        if (dir && dir_size) dir[0] = '\0';
        if (name && name_size) name[0] = '\0';
        return;
    }
    const char* last_slash = NULL;
    for (const char* p = path; *p; ++p) {
        if (*p == '/') last_slash = p;
    }
    if (last_slash) {
        size_t dir_len = (size_t)(last_slash - path);
        if (dir && dir_size) {
            size_t copy_len = dir_len < dir_size - 1 ? dir_len : dir_size - 1;
            for (size_t i = 0; i < copy_len; ++i) dir[i] = path[i];
            dir[copy_len] = '\0';
        }
        if (name && name_size) {
            size_t copy_len = 0;
            const char* fname = last_slash + 1;
            while (fname[copy_len] && copy_len + 1 < name_size) {
                name[copy_len] = fname[copy_len];
                ++copy_len;
            }
            name[copy_len] = '\0';
        }
    } else {
        // No slash found: dir is empty, name is path
        if (dir && dir_size) dir[0] = '\0';
        if (name && name_size) {
            size_t copy_len = 0;
            while (path[copy_len] && copy_len + 1 < name_size) {
                name[copy_len] = path[copy_len];
                ++copy_len;
            }
            name[copy_len] = '\0';
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif // FS_UTILS_H