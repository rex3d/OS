#include "superblock.h"
#include "vfs.h"
#include <string.h>

#define MAX_SUPERBLOCKS 8

// Statyczna tablica wskaźników na zarejestrowane superbloki
static struct superblock *superblock_table[MAX_SUPERBLOCKS] = {0};

// Rejestruje superblok w tablicy
// Zwraca 0 jeśli się udało, -1 gdy brak miejsca
int register_superblock(struct superblock *sb) {
    if (!sb || !sb->name) return -2; // walidacja
    for (int i = 0; i < MAX_SUPERBLOCKS; i++) {
        if (!superblock_table[i]) {
            superblock_table[i] = sb;
            return 0;
        }
    }
    return -1; // brak miejsca w tablicy
}

// Znajduje zarejestrowany superblok po nazwie
// Zwraca wskaźnik lub NULL jeśli nie ma
struct superblock *get_superblock_by_name(const char *name) {
    if (!name) return NULL;
    for (int i = 0; i < MAX_SUPERBLOCKS; i++) {
        if (superblock_table[i] && strcmp(superblock_table[i]->name, name) == 0) {
            return superblock_table[i];
        }
    }
    return NULL;
}
