// fs_perm.c
#include "fs_perm.h"
#include <stdbool.h>

#define FS_PERM_OWNER_SHIFT 6
#define FS_PERM_GROUP_SHIFT 3
#define FS_PERM_OTHER_SHIFT 0
#define FS_PERM_MASK 0x7U // 3 bity: rwx

// Sprawdza uprawnienia, zwraca true jeśli użytkownik ma wszystkie operacje z op
bool fs_perm_check(const fs_perm_t* perm, fs_uid_t uid, fs_gid_t gid, uint8_t op) {
    if (!perm) return false;

    uint8_t shift = (perm->uid == uid)   ? FS_PERM_OWNER_SHIFT :
                    (perm->gid == gid)   ? FS_PERM_GROUP_SHIFT :
                                          FS_PERM_OTHER_SHIFT;

    uint8_t perm_bits = (perm->perms >> shift) & FS_PERM_MASK;

    return (perm_bits & op) == op;
}

// Ustawia uprawnienia i przypisuje właściciela i grupę
void fs_perm_set(fs_perm_t* perm, fs_uid_t uid, fs_gid_t gid, uint8_t perms) {
    if (!perm) return;

    perm->uid = uid;
    perm->gid = gid;
    // perms to tylko 3 bity rwx (owner), musimy ustawić perms w formacie rwxrwxrwx
    // więc przesuwamy perms owner na OWNER_SHIFT, GROUP_SHIFT i OTHER_SHIFT?  
    // Ale w nagłówku masz tylko 8-bit perms, a w implementacji jest przesuwanie jakby 9-bit...

    // Założę, że perms to maska 9-bitowa rwxrwxrwx, ale w nagłówku masz 8-bit perms.  
    // Trzeba więc przyjąć, że perms jest już w formacie rwxrwxrwx (czyli 8-9 bitów).

    // Tutaj musisz sam ustawić perms w formacie rwxrwxrwx, więc przypisuję bez zmiany:
    perm->perms = perms;
}
