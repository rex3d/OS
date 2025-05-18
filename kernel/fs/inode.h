#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "fs_perm.h"

#ifdef __cplusplus
extern "C" {
#endif

// Przenośność typów dla numeru i-węzła i rozmiaru pliku
#if INTPTR_MAX == INT64_MAX
    typedef uint64_t inode_num_t;
    typedef uint64_t inode_size_t;
#elif INTPTR_MAX == INT32_MAX
    typedef uint32_t inode_num_t;
    typedef uint32_t inode_size_t;
#else
    #error "Unsupported architecture"
#endif

typedef enum {
    INODE_TYPE_FILE = 0,
    INODE_TYPE_DIR,
    INODE_TYPE_SYMLINK,
    INODE_TYPE_DEV
} inode_type_t;

// Struktura i-węzła zoptymalizowana pod względem wyrównania i przenośności
typedef struct inode {
    inode_num_t ino;            // Unikalny numer i-węzła
    inode_type_t type;          // Typ i-węzła
    inode_size_t size;          // Rozmiar pliku
    uint32_t nlink;             // Liczba dowiązań
    fs_perm_t perm;             // Uprawnienia
    void* fs_data;              // Wskaźnik na dane systemu plików
    struct inode* parent;       // Rodzic (dla katalogów)
    struct inode* next;         // Następny w globalnej liście
} inode_t;

// Tworzy nowy i-węzeł
inode_t* inode_create(inode_num_t ino, inode_type_t type, inode_size_t size, fs_perm_t perm, void* fs_data);

// Zwalnia i-węzeł
void inode_destroy(inode_t* node);

// Wyszukuje i-węzeł po numerze
inode_t* inode_find(inode_num_t ino);

// Dodaje i-węzeł do globalnej listy
void inode_register(inode_t* node);

// Usuwa i-węzeł z globalnej listy
void inode_unregister(inode_t* node);

#ifdef __cplusplus
}
#endif

#endif // INODE_H