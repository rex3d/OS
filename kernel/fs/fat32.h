#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAT32_MAX_FILENAME 11

typedef struct __attribute__((packed)) {
    uint8_t  name[FAT32_MAX_FILENAME];   // 8+3 znaków (bez null-terminatora)
    uint8_t  attr;
    uint8_t  nt_reserved;
    uint8_t  creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t size;
} fat32_dir_entry_t;

_Static_assert(sizeof(fat32_dir_entry_t) == 32, "fat32_dir_entry_t must be 32 bytes");

typedef struct {
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint32_t sectors_per_fat;
    uint32_t root_cluster;
    uint32_t fat_start;
    uint32_t data_start;
    uint32_t total_sectors;
} fat32_fs_t;

int fat32_mount(int (*read_sector)(uint32_t lba, void* buf));
int fat32_open(const char* filename, fat32_dir_entry_t* entry);
int fat32_read(const fat32_dir_entry_t* entry, void* buf, size_t count, size_t* read_count);

#ifdef __cplusplus
}
#endif

#endif // FAT32_H
