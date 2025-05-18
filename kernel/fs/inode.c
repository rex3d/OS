#include "inode.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

static inode_t* inode_list_head = NULL;

// Tworzy nowy inode i rejestruje go w globalnej liście
inode_t* inode_create(uint64_t ino, inode_type_t type, uint64_t size, fs_perm_t perm, void* fs_data) {
    inode_t* node = (inode_t*)calloc(1, sizeof(inode_t));
    if (!node) return NULL;
    node->ino = ino;
    node->type = type;
    node->size = size;
    node->nlink = 1;
    node->perm = perm;
    node->fs_data = fs_data;
    inode_register(node);
    return node;
}

// Usuwa inode z listy i zwalnia pamięć
void inode_destroy(inode_t* node) {
    if (!node) return;
    inode_unregister(node);
    // Jeśli fs_data wymaga zwolnienia, zrób to tutaj
    free(node);
}

// Wyszukuje inode po numerze
inode_t* inode_find(uint64_t ino) {
    for (inode_t* node = inode_list_head; node; node = node->next) {
        if (node->ino == ino)
            return node;
    }
    return NULL;
}

// Rejestruje inode w globalnej liście (na początku listy)
void inode_register(inode_t* node) {
    if (!node) return;
    node->next = inode_list_head;
    inode_list_head = node;
}

// Usuwa inode z globalnej listy
void inode_unregister(inode_t* node) {
    if (!node) return;
    inode_t **prev = &inode_list_head;
    while (*prev) {
        if (*prev == node) {
            *prev = node->next;
            node->next = NULL;
            return;
        }
        prev = &(*prev)->next;
    }
}

// Opcjonalnie: funkcja do czyszczenia wszystkich inode'ów (np. przy zamykaniu systemu plików)
void inode_cleanup_all(void) {
    inode_t* node = inode_list_head;
    while (node) {
        inode_t* next = node->next;
        inode_destroy(node);
        node = next;
    }
    inode_list_head = NULL;
}
