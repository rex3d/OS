#include "mount.h"
#include <string.h>
#include <stdlib.h>

static mount_point_t* mount_list_head = NULL;

int mount_fs(const char* path, fs_type_t type, void* fs_data) {
    if (!path || !fs_data) return -1;

    // Sprawdź, czy już zamontowane
    for (mount_point_t* m = mount_list_head; m; m = m->next) {
        if (strcmp(m->path, path) == 0)
            return -2; // Już zamontowane
    }

    filesystem_t* fs = (filesystem_t*)malloc(sizeof(filesystem_t));
    if (!fs) return -3;

    fs->type = type;
    fs->fs_data = fs_data;
    strncpy(fs->mount_point, path, sizeof(fs->mount_point) - 1);
    fs->mount_point[sizeof(fs->mount_point) - 1] = '\0';
    fs->next = NULL;

    mount_point_t* mp = (mount_point_t*)malloc(sizeof(mount_point_t));
    if (!mp) {
        free(fs);
        return -4;
    }

    strncpy(mp->path, path, sizeof(mp->path) - 1);
    mp->path[sizeof(mp->path) - 1] = '\0';
    mp->fs = fs;
    mp->next = mount_list_head;
    mount_list_head = mp;

    return 0;
}

int umount_fs(const char* path) {
    if (!path) return -1;

    mount_point_t** prev = &mount_list_head;
    mount_point_t* mp = mount_list_head;

    while (mp) {
        if (strcmp(mp->path, path) == 0) {
            *prev = mp->next;
            if (mp->fs) free(mp->fs);
            free(mp);
            return 0;
        }
        prev = &mp->next;
        mp = mp->next;
    }

    return -2; // Nie znaleziono punktu montowania
}

mount_point_t* mount_find(const char* path) {
    mount_point_t* best = NULL;
    size_t best_len = 0;

    for (mount_point_t* mp = mount_list_head; mp; mp = mp->next) {
        size_t len = strlen(mp->path);
        if (len <= strlen(path) && strncmp(path, mp->path, len) == 0) {
            // Znajdujemy najdłuższy pasujący prefix
            if (len > best_len) {
                best = mp;
                best_len = len;
            }
        }
    }

    return best;
}

void mount_list(void (*cb)(const mount_point_t*, void*), void* user_data) {
    for (mount_point_t* mp = mount_list_head; mp; mp = mp->next) {
        cb(mp, user_data);
    }
}
void mount_free_all() {
    mount_point_t* mp = mount_list_head;
    while (mp) {
        mount_point_t* next = mp->next;
        if (mp->fs) free(mp->fs);
        free(mp);
        mp = next;
    }
    mount_list_head = NULL;
}