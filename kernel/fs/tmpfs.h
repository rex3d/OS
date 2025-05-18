#ifndef TMPFS_H
#define TMPFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs.h"

// Montuje tmpfs do podanego węzła root
int tmpfs_mount(vfs_node_t *root, void *device);

// "Formatuje" tmpfs – RAM, więc zwykle nic nie robi
int tmpfs_format(void *device);

#ifdef __cplusplus
}
#endif

#endif // TMPFS_H
