#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stddef.h>

// Maksymalna długość nazwy pliku 11 (8+3 format, bez kropki)
#define FAT_MAX_FILENAME 11

typedef enum {
    FAT_TYPE_NONE = 0,
    FAT_TYPE_12,
    FAT_TYPE_16,
    FAT_TYPE_32
} fat_type_t;

typedef struct fat_dir_entry_s {
    uint8_t  name[11];
    uint8_t  attr;
    uint8_t  reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster_hi;  // FAT32 only
    uint16_t first_cluster_lo;
    uint32_t size;
} __attribute__((packed)) fat_dir_entry_t;

typedef struct fat_fs_s {
    fat_type_t type;

    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;    // FAT12/16 only
    uint32_t total_sectors;
    uint8_t  media_type;
    uint16_t sectors_per_fat_16;  // FAT12/16
    uint32_t sectors_per_fat_32;  // FAT32
    uint32_t fat_start;
    uint32_t root_start;
    uint32_t data_start;
    uint32_t total_clusters;

    // Funkcja do czytania sektora z dysku/obrazu
    int (*read_sector)(uint32_t lba, void* buf);
} fat_fs_t;

// Mountuje FAT, rozpoznaje typ FAT i inicjuje struktury.
// Zwraca 0 na sukces, -1 na błąd.
int fat_mount(fat_fs_t* fs, int (*read_sector)(uint32_t lba, void* buf));

// Otwiera plik po nazwie (11 znaków, bez kropki, wielkość liter jak w FAT).
// Zwraca 0 na sukces, -1 na błąd.
int fat_open(fat_fs_t* fs, const char* filename, fat_dir_entry_t* entry);

// Czyta plik podany przez wpis katalogowy, bufor i ilość bajtów.
// read_count zwraca ilość przeczytanych bajtów.
// Zwraca 0 na sukces, -1 na błąd.
int fat_read(fat_fs_t* fs, const fat_dir_entry_t* entry, void* buf, size_t count, size_t* read_count);

#endif // FAT_H
