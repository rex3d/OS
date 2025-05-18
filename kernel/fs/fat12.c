#include "fat12.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifndef FAT12_SECTOR_SIZE
#define FAT12_SECTOR_SIZE 512
#endif

#define LE16(x) ((uint16_t)((x)[0] | ((x)[1] << 8)))
#define LE32(x) ((uint32_t)((x)[0] | ((x)[1] << 8) | ((x)[2] << 16) | ((x)[3] << 24)))

typedef struct {
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_large;
} __attribute__((packed)) fat12_bpb_t;

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
    uint32_t total_clusters;
} fat12_fs_t;

static fat12_fs_t fat12_fs;
static int (*fat12_read_sector)(uint32_t, void*) = NULL;

int fat12_mount(int (*read_sector)(uint32_t lba, void* buf)) {
    uint8_t bpb_raw[36];
    if (!read_sector) return -1;
    fat12_read_sector = read_sector;
    if (fat12_read_sector(0, bpb_raw)) return -1;

    fat12_fs.bytes_per_sector    = LE16(&bpb_raw[11]);
    fat12_fs.sectors_per_cluster = bpb_raw[13];
    fat12_fs.reserved_sectors    = LE16(&bpb_raw[14]);
    fat12_fs.num_fats            = bpb_raw[16];
    fat12_fs.root_entries        = LE16(&bpb_raw[17]);
    fat12_fs.total_sectors       = LE16(&bpb_raw[19]);
    fat12_fs.media_type          = bpb_raw[21];
    fat12_fs.sectors_per_fat     = LE16(&bpb_raw[22]);

    fat12_fs.fat_start = fat12_fs.reserved_sectors;
    fat12_fs.root_start = fat12_fs.fat_start + fat12_fs.num_fats * fat12_fs.sectors_per_fat;
    fat12_fs.data_start = fat12_fs.root_start + ((fat12_fs.root_entries * 32 + fat12_fs.bytes_per_sector - 1) / fat12_fs.bytes_per_sector);
    fat12_fs.total_clusters = (fat12_fs.total_sectors - fat12_fs.data_start) / fat12_fs.sectors_per_cluster;

    return 0;
}

int fat12_open(const char* filename, fat12_dir_entry_t* entry) {
    uint8_t sector[FAT12_SECTOR_SIZE];
    uint32_t root_dir_sectors = ((fat12_fs.root_entries * 32) + (fat12_fs.bytes_per_sector - 1)) / fat12_fs.bytes_per_sector;

    for (uint32_t i = 0; i < root_dir_sectors; ++i) {
        if (fat12_read_sector(fat12_fs.root_start + i, sector)) return -1;
        for (uint32_t j = 0; j < fat12_fs.bytes_per_sector / 32; ++j) {
            fat12_dir_entry_t* e = (fat12_dir_entry_t*)(sector + j * 32);
            if (e->name[0] == 0x00) return -1; // koniec katalogu
            if (e->name[0] == 0xE5) continue;  // skasowany
            if (memcmp(e->name, filename, FAT12_MAX_FILENAME) == 0) {
                memcpy(entry, e, sizeof(fat12_dir_entry_t));
                return 0;
            }
        }
    }
    return -1;
}

static uint16_t fat12_get_fat_entry(uint32_t cluster, uint8_t* fat_buf) {
    uint32_t offset = (cluster * 3) / 2;
    uint16_t value;

    if (cluster & 1) {
        value = ((fat_buf[offset] >> 4) | (fat_buf[offset + 1] << 4)) & 0x0FFF;
    } else {
        value = (fat_buf[offset] | ((fat_buf[offset + 1] & 0x0F) << 8)) & 0x0FFF;
    }
    return value;
}

int fat12_read(const fat12_dir_entry_t* entry, void* buf, size_t count, size_t* read_count) {
    if (!entry || !buf) return -1;

    uint32_t cluster = entry->first_cluster_lo;
    uint32_t bytes_left = entry->size;
    size_t total_read = 0;
    size_t to_read = (count < bytes_left) ? count : bytes_left;

    uint8_t fat_buf[FAT12_SECTOR_SIZE * 2];
    for (uint32_t i = 0; i < fat12_fs.sectors_per_fat; ++i) {
        if (fat12_read_sector(fat12_fs.fat_start + i, fat_buf + i * fat12_fs.bytes_per_sector)) return -1;
    }

    uint8_t sector_buf[FAT12_SECTOR_SIZE];
    while (cluster >= 2 && cluster < 0xFF8 && total_read < to_read) {
        for (uint8_t s = 0; s < fat12_fs.sectors_per_cluster; ++s) {
            uint32_t sector = fat12_fs.data_start + (cluster - 2) * fat12_fs.sectors_per_cluster + s;
            if (fat12_read_sector(sector, sector_buf)) return -1;

            size_t chunk = fat12_fs.bytes_per_sector;
            if (chunk > to_read - total_read) chunk = to_read - total_read;
            memcpy((uint8_t*)buf + total_read, sector_buf, chunk);
            total_read += chunk;
            if (total_read >= to_read) break;
        }
        cluster = fat12_get_fat_entry(cluster, fat_buf);
    }

    if (read_count) *read_count = total_read;
    return 0;
}
