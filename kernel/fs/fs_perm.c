// fs_perm.c
#include "fs_perm.h"
#include <stdint.h>
#include <stdbool.h>

#define FS_PERM_OWNER_SHIFT 6
#define FS_PERM_GROUP_SHIFT 3
#define FS_PERM_OTHER_SHIFT 0
#define FS_PERM_MASK 0x7U      // 3 bity: rwx
#define FS_PERM_ALL_MASK 0x1FFU // 9 bitów

// Sprawdza uprawnienia dla danego użytkownika i grupy na operację op (bit maska)
bool fs_perm_check(const fs_perm_t* perm, uint32_t uid, uint32_t gid, uint8_t op) {
    if (!perm) return false;
    uint8_t shift = (perm->uid == uid) ? FS_PERM_OWNER_SHIFT :
                    (perm->gid == gid) ? FS_PERM_GROUP_SHIFT :
                                         FS_PERM_OTHER_SHIFT;
    uint8_t perm_bits = (perm->perms >> shift) & FS_PERM_MASK;
    return (perm_bits & op) == op;
}

// Ustawia uprawnienia, przymaskowane do 9 bitów
void fs_perm_set(fs_perm_t* perm, uint32_t uid, uint32_t gid, uint16_t perms) {
    if (!perm) return;
    perm->uid = uid;
    perm->gid = gid;
    perm->perms = perms & FS_PERM_ALL_MASK;
}
// Ustawia uprawnienia dla właściciela
void fs_perm_set_owner(fs_perm_t* perm, uint16_t perms) {
    if (!perm) return;
    perm->perms = (perm->perms & ~(FS_PERM_MASK << FS_PERM_OWNER_SHIFT)) |
                  ((perms & FS_PERM_MASK) << FS_PERM_OWNER_SHIFT);
}
// Ustawia uprawnienia dla grupy
void fs_perm_set_group(fs_perm_t* perm, uint16_t perms) {
    if (!perm) return;
    perm->perms = (perm->perms & ~(FS_PERM_MASK << FS_PERM_GROUP_SHIFT)) |
                  ((perms & FS_PERM_MASK) << FS_PERM_GROUP_SHIFT);
}
// Ustawia uprawnienia dla innych
void fs_perm_set_other(fs_perm_t* perm, uint16_t perms) {
    if (!perm) return;
    perm->perms = (perm->perms & ~(FS_PERM_MASK << FS_PERM_OTHER_SHIFT)) |
                  ((perms & FS_PERM_MASK) << FS_PERM_OTHER_SHIFT);
}
// Ustawia uprawnienia dla wszystkich
void fs_perm_set_all(fs_perm_t* perm, uint16_t perms) {
    if (!perm) return;
    perm->perms = perms & FS_PERM_ALL_MASK;
}
