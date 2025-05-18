#include "vfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_PERMISSIONS 0777

static filesystem_t *fs_table[VFS_MAX_FS];
static uint32_t next_inode = 1;

static vfs_node_t vfs_root = {
    .name = "/",
    .inode = 0,
    .permissions = DEFAULT_PERMISSIONS,
    .is_dir = 1,
    .parent = NULL,
    .children = NULL,
    .next = NULL,
    .fs_data = NULL
};

// kmalloc z zerowaniem
void* kmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        printf("kmalloc failed\n");
        return NULL;
    }
    memset(ptr, 0, size);
    return ptr;
}

int vfs_register_fs(filesystem_t *fs) {
    for (int i = 0; i < VFS_MAX_FS; i++) {
        if (!fs_table[i]) {
            fs_table[i] = fs;
            return 0;
        }
    }
    return -1;
}

vfs_node_t *vfs_lookup(const char *path) {
    if (!path || path[0] != '/') return NULL;
    if (strcmp(path, "/") == 0) return &vfs_root;

    char temp[VFS_PATH_MAX];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    vfs_node_t *current = &vfs_root;
    char *token = strtok(temp, "/");

    while (token && current) {
        vfs_node_t *child = current->children;
        while (child) {
            if (strcmp(child->name, token) == 0) {
                current = child;
                break;
            }
            child = child->next;
        }
        if (!child) return NULL;
        token = strtok(NULL, "/");
    }

    return current;
}

static vfs_node_t* vfs_create_node(const char *name, uint8_t is_dir, vfs_node_t *parent) {
    vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    if (!node) return NULL;

    strncpy(node->name, name, sizeof(node->name) - 1);
    node->inode = next_inode++;
    node->permissions = DEFAULT_PERMISSIONS;
    node->is_dir = is_dir;
    node->parent = parent;

    // dodaj na początek listy dzieci
    node->next = parent->children;
    parent->children = node;

    return node;
}

int vfs_mkdir(const char *path) {
    if (!path || path[0] != '/') return -1;

    char temp[VFS_PATH_MAX];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    vfs_node_t *current = &vfs_root;
    char *token = strtok(temp, "/");

    while (token) {
        vfs_node_t *child = current->children;
        while (child) {
            if (strcmp(child->name, token) == 0 && child->is_dir) {
                current = child;
                break;
            }
            child = child->next;
        }

        if (!child) {
            vfs_node_t *new_node = vfs_create_node(token, 1, current);
            if (!new_node) return -1;
            current = new_node;
        }

        token = strtok(NULL, "/");
    }

    return 0;
}

int vfs_create(const char *path) {
    if (!path || path[0] != '/') return -1;

    char temp[VFS_PATH_MAX];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char *last = strrchr(temp, '/');
    if (!last || last == temp) return -1;

    *last = '\0';
    const char *filename = last + 1;

    vfs_node_t *dir = vfs_lookup(temp[0] ? temp : "/");
    if (!dir || !dir->is_dir) return -1;

    // sprawdź, czy plik już istnieje
    vfs_node_t *child = dir->children;
    while (child) {
        if (strcmp(child->name, filename) == 0)
            return -1;
        child = child->next;
    }

    vfs_node_t *new_node = vfs_create_node(filename, 0, dir);
    return new_node ? 0 : -1;
}

int vfs_mount(const char *fs_name, const char *mount_point, void *device) {
    vfs_node_t *mount_node = vfs_lookup(mount_point);
    if (!mount_node || !mount_node->is_dir) return -1;

    for (int i = 0; i < VFS_MAX_FS; i++) {
        if (fs_table[i] && strcmp(fs_table[i]->name, fs_name) == 0) {
            if (fs_table[i]->format)
                fs_table[i]->format(device);
            return fs_table[i]->mount(mount_node, device);
        }
    }

    return -1;
}
