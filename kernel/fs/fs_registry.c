#include "fs_registry.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define FS_NAME_MAX 31

typedef struct fs_registry_entry {
    char fs_name[FS_NAME_MAX + 1];
    fs_mount_fn mount_fn;
    struct fs_registry_entry* next;
} fs_registry_entry_t;

static fs_registry_entry_t* fs_registry_list_head = NULL;

// Register a new filesystem
int fs_registry_register(const char* fs_name, fs_mount_fn mount_fn) {
    if (!fs_name || !mount_fn) return -1;
    if (strlen(fs_name) > FS_NAME_MAX) return -3;

    // Check for duplicates
    fs_registry_entry_t* curr = fs_registry_list_head;
    while (curr) {
        if (strcmp(curr->fs_name, fs_name) == 0)
            return -4; // Already registered
        curr = curr->next;
    }

    fs_registry_entry_t* entry = (fs_registry_entry_t*)malloc(sizeof(fs_registry_entry_t));
    if (!entry) return -2;
    strncpy(entry->fs_name, fs_name, FS_NAME_MAX);
    entry->fs_name[FS_NAME_MAX] = '\0';
    entry->mount_fn = mount_fn;
    entry->next = fs_registry_list_head;
    fs_registry_list_head = entry;
    return 0;
}

// Unregister a filesystem
int fs_registry_unregister(const char* fs_name) {
    if (!fs_name) return -1;
    fs_registry_entry_t **pp = &fs_registry_list_head;
    while (*pp) {
        if (strcmp((*pp)->fs_name, fs_name) == 0) {
            fs_registry_entry_t* to_delete = *pp;
            *pp = to_delete->next;
            free(to_delete);
            return 0;
        }
        pp = &((*pp)->next);
    }
    return -2; // Not found
}

// Find a filesystem mount function
fs_mount_fn fs_registry_find(const char* fs_name) {
    if (!fs_name) return NULL;
    fs_registry_entry_t* entry = fs_registry_list_head;
    while (entry) {
        if (strcmp(entry->fs_name, fs_name) == 0)
            return entry->mount_fn;
        entry = entry->next;
    }
    return NULL;
}

// List all registered filesystems
void fs_registry_list(void (*cb)(const char* fs_name, fs_mount_fn fn, void* user_data), void* user_data) {
    if (!cb) return;
    fs_registry_entry_t* entry = fs_registry_list_head;
    while (entry) {
        cb(entry->fs_name, entry->mount_fn, user_data);
        entry = entry->next;
    }
}

// Optional: Cleanup all registry entries (call at program exit)
void fs_registry_cleanup(void) {
    fs_registry_entry_t* entry = fs_registry_list_head;
    while (entry) {
        fs_registry_entry_t* next = entry->next;
        free(entry);
        entry = next;
    }
    fs_registry_list_head = NULL;
}