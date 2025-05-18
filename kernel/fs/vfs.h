#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

#define VFS_PATH_MAX 256
#define VFS_MAX_FS   8

typedef struct vfs_node {
    char name[64];
    uint32_t inode;
    uint32_t permissions;
    uint8_t is_dir;

    struct vfs_node *parent;
    struct vfs_node *children;
    struct vfs_node *next;

    void *fs_data;
} vfs_node_t;

typedef struct filesystem {
    const char *name;
    int (*mount)(vfs_node_t *root, void *device);
    int (*format)(void *device);
} filesystem_t;

int vfs_register_fs(filesystem_t *fs);
int vfs_mount(const char *fs_name, const char *mount_point, void *device);
vfs_node_t *vfs_lookup(const char *path);
int vfs_mkdir(const char *path);
int vfs_create(const char *path);

#endif
