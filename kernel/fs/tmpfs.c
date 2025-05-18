#include "tmpfs.h"
#include "vfs.h"
#include "kheap.h"  // zakładam, że tu masz kmalloc
#include <string.h>

int tmpfs_mount(vfs_node_t *root, void *device) {
    if (!root) return -1;

    vfs_node_t *tmp_root = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    if (!tmp_root) return -2;

    memset(tmp_root, 0, sizeof(vfs_node_t));

    strncpy(tmp_root->name, "tmp", sizeof(tmp_root->name) - 1);
    tmp_root->name[sizeof(tmp_root->name) - 1] = '\0';

    tmp_root->is_dir = 1;
    tmp_root->parent = root;
    tmp_root->fs = root->fs;  // dziedziczy ten sam fs co root

    tmp_root->children = NULL;
    tmp_root->next = root->children;
    root->children = tmp_root;

    return 0;
}

int tmpfs_format(void *device) {
    (void)device; // Niepotrzebne, ale żeby kompilator nie sapał
    return 0;     // RAM nie trzeba formatować
}
