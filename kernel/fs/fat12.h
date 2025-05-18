#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>
#include <stddef.h>

#define FAT12_MAX_FILENAME 11

typedef struct {
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint32_t fat_start;
    uint32_t root_start;
    uint32_t data_start;
} fat12_fs_t;

typedef struct {
    uint8_t  name[11];
    uint8_t  attr;
    uint8_t  reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster;
    uint32_t size;
} __attribute__((packed)) fat12_dir_entry_t;

int fat12_mount(int (*read_sector)(uint32_t lba, void* buf));
int fat12_open(const char* filename, fat12_dir_entry_t* entry);
int fat12_read(const fat12_dir_entry_t* entry, void* buf, size_t count, size_t* read_count);

#endif // FAT12_H
