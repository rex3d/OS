// filesystem.c
#include "filesystem.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

static filesystem_t* fs_list_head = NULL;

// Helper: porównuje mount pointy bezpiecznie, uwzględniając limit
static int mount_point_cmp(const char* a, const char* b) {
    return strncmp(a, b, FS_MOUNT_POINT_MAX);
}

int filesystem_mount(fs_type_t type, const char* mount_point, void* fs_data) {
    if (!mount_point || strlen(mount_point) >= FS_MOUNT_POINT_MAX)
        return -1; // Niepoprawny mount point

    // Sprawdź, czy już zamontowany
    for (filesystem_t* fs = fs_list_head; fs; fs = fs->next) {
        if (mount_point_cmp(fs->mount_point, mount_point) == 0)
            return -2; // Już zamontowany
    }

    filesystem_t* fs = (filesystem_t*)calloc(1, sizeof(filesystem_t));
    if (!fs)
        return -3; // Brak pamięci

    fs->type = type;
    strncpy(fs->mount_point, mount_point, FS_MOUNT_POINT_MAX - 1);
    fs->mount_point[FS_MOUNT_POINT_MAX - 1] = '\0';
    fs->fs_data = fs_data;

    // Wstaw na początek listy
    fs->next = fs_list_head;
    fs_list_head = fs;

    return 0;
}

int filesystem_unmount(const char* mount_point) {
    if (!mount_point)
        return -1;

    filesystem_t** prev = &fs_list_head;
    for (filesystem_t* fs = fs_list_head; fs; fs = fs->next) {
        if (mount_point_cmp(fs->mount_point, mount_point) == 0) {
            *prev = fs->next;
            free(fs);
            return 0;
        }
        prev = &fs->next;
    }
    return -2; // Nie znaleziono mount pointa
}

const filesystem_t* filesystem_find(const char* path) {
    if (!path)
        return NULL;

    filesystem_t* best = NULL;
    size_t best_len = 0;
    for (filesystem_t* fs = fs_list_head; fs; fs = fs->next) {
        size_t len = strnlen(fs->mount_point, FS_MOUNT_POINT_MAX);
        if (len == 0)
            continue;

        if (strncmp(path, fs->mount_point, len) == 0 &&
            (path[len] == '/' || path[len] == '\0')) {
            if (len > best_len) {
                best = fs;
                best_len = len;
            }
        }
    }
    return best;
}

void filesystem_list(void (*cb)(const filesystem_t* fs, void* user_data), void* user_data) {
    for (filesystem_t* fs = fs_list_head; fs; fs = fs->next) {
        cb(fs, user_data);
    }
}

size_t filesystem_count(void) {
    size_t count = 0;
    for (filesystem_t* fs = fs_list_head; fs; fs = fs->next) {
        count++;
    }
    return count;
}
