#include "fat16.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#if defined(_MSC_VER)
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#else
#define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#endif

STATIC_ASSERT(sizeof(uint8_t) == 1, "uint8_t size error");
STATIC_ASSERT(sizeof(uint16_t) == 2, "uint16_t size error");
STATIC_ASSERT(sizeof(uint32_t) == 4, "uint32_t size error");

#define FAT16_MAX_SECTOR_SIZE  512
#define FAT16_ENTRY_SIZE       32
#define FAT16_CLUSTER_EOF      0xFFF8
#define FAT16_CLUSTER_BAD      0xFFF7
#define FAT16_CLUSTER_MIN      2
#define FAT16_CLUSTER_MAX      0xFFEF
#define FAT16_MAX_CHAIN_LENGTH 0x10000 // zapobiega zapętleniu

static fat16_fs_t fat16_fs;
static int (*fat16_read_sector)(uint32_t, void*) = NULL;

static uint16_t fat16_get_u16(const uint8_t* p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t fat16_get_u32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t fat16_total_sectors(const uint8_t* bpb) {
    uint16_t ts16 = fat16_get_u16(bpb + 19);
    if (ts16 != 0) return ts16;
    return fat16_get_u32(bpb + 32);
}

static int fat16_is_valid_bpb(const uint8_t* bpb) {
    if (bpb[510] != 0x55 || bpb[511] != 0xAA) return 0;
    uint16_t bps = fat16_get_u16(bpb + 11);
    if (!(bps == 512 || bps == 1024 || bps == 2048 || bps == 4096)) return 0;
    if (bpb[13] == 0) return 0;
    if (bpb[16] == 0) return 0;
    if (fat16_get_u16(bpb + 22) == 0) return 0; // sektory na FAT muszą być
    return 1;
}

int fat16_mount(int (*read_sector)(uint32_t lba, void* buf)) {
    if (!read_sector) return -1;
    fat16_read_sector = read_sector;

    uint8_t bpb[FAT16_MAX_SECTOR_SIZE] = {0};
    if (fat16_read_sector(0, bpb)) return -2;
    if (!fat16_is_valid_bpb(bpb)) return -3;

    fat16_fs.bytes_per_sector    = fat16_get_u16(bpb + 11);
    fat16_fs.sectors_per_cluster = bpb[13];
    fat16_fs.reserved_sectors    = fat16_get_u16(bpb + 14);
    fat16_fs.num_fats            = bpb[16];
    fat16_fs.root_entries        = fat16_get_u16(bpb + 17);
    fat16_fs.total_sectors       = fat16_total_sectors(bpb);
    fat16_fs.media_type          = bpb[21];
    fat16_fs.sectors_per_fat     = fat16_get_u16(bpb + 22);

    fat16_fs.fat_start  = fat16_fs.reserved_sectors;
    fat16_fs.root_sectors = (fat16_fs.root_entries * FAT16_ENTRY_SIZE + fat16_fs.bytes_per_sector - 1) / fat16_fs.bytes_per_sector;
    fat16_fs.root_start = fat16_fs.fat_start + fat16_fs.num_fats * fat16_fs.sectors_per_fat;
    fat16_fs.data_start = fat16_fs.root_start + fat16_fs.root_sectors;

    // Podstawowa walidacja rozsianych wartości
    if (fat16_fs.bytes_per_sector > FAT16_MAX_SECTOR_SIZE) return -4;
    if (fat16_fs.sectors_per_cluster == 0) return -5;
    if (fat16_fs.total_sectors == 0) return -6;

    return 0;
}

// Porównanie nazwy pliku 8.3 z potencjalną nazwą użytkownika, case-insensitive
static int fat16_match_filename(const uint8_t* entry_name, const char* filename) {
    char name[9] = {0}, ext[4] = {0}, full[13] = {0};
    memcpy(name, entry_name, 8);
    memcpy(ext, entry_name + 8, 3);
    for (int i = 7; i >= 0 && name[i] == ' '; --i) name[i] = 0;
    for (int i = 2; i >= 0 && ext[i] == ' '; --i) ext[i] = 0;
    if (ext[0])
        snprintf(full, sizeof(full), "%s.%s", name, ext);
    else
        snprintf(full, sizeof(full), "%s", name);
    return strcasecmp(full, filename) == 0;
}

int fat16_open(const char* filename, fat16_dir_entry_t* entry) {
    if (!filename || !entry) return -1;
    uint8_t sector[FAT16_MAX_SECTOR_SIZE];

    for (uint32_t i = 0; i < fat16_fs.root_sectors; ++i) {
        if (fat16_read_sector(fat16_fs.root_start + i, sector)) return -2;

        for (uint32_t j = 0; j < fat16_fs.bytes_per_sector / FAT16_ENTRY_SIZE; ++j) {
            fat16_dir_entry_t* e = (fat16_dir_entry_t*)(sector + j * FAT16_ENTRY_SIZE);
            if (e->name[0] == 0x00) return -3; // koniec katalogu
            if (e->name[0] == 0xE5) continue;  // usunięty plik
            if ((e->attr & 0x0F) == 0x0F) continue; // LFN, olewamy

            if (fat16_match_filename(e->name, filename)) {
                memcpy(entry, e, sizeof(fat16_dir_entry_t));
                return 0;
            }
        }
    }
    return -4; // Nie znaleziono pliku
}

static int fat16_read_fat_entry(uint16_t cluster, uint16_t* value) {
    if (!value) return -1;
    if (cluster < FAT16_CLUSTER_MIN) return -2; // nieprawidłowy klaster
    uint32_t fat_offset = cluster * 2;
    uint32_t sector_num = fat16_fs.fat_start + (fat_offset / fat16_fs.bytes_per_sector);
    uint8_t sector[FAT16_MAX_SECTOR_SIZE];
    if (fat16_read_sector(sector_num, sector)) return -3;
    uint32_t offset = fat_offset % fat16_fs.bytes_per_sector;
    *value = (uint16_t)(sector[offset] | (sector[offset + 1] << 8));
    return 0;
}

// Lepsze czytanie pliku - wiele sektorów na raz, bez powielonego memcpy, z obsługą błędów i limitów
int fat16_read(const fat16_dir_entry_t* entry, void* buf, size_t count, size_t* read_count) {
    if (!entry || !buf) return -1;
    size_t to_read = count > entry->size ? entry->size : count;
    size_t total_read = 0;
    uint16_t cluster = entry->first_cluster;
    size_t cluster_size = fat16_fs.bytes_per_sector * fat16_fs.sectors_per_cluster;

    // Żeby nie wpaść w zapętloną listę klastrów
    size_t chain_length = 0;

    while (to_read > 0 && cluster >= FAT16_CLUSTER_MIN && cluster < FAT16_CLUSTER_EOF) {
        if (++chain_length > FAT16_MAX_CHAIN_LENGTH) return -5; // zapętlenie

        uint32_t first_sector = fat16_fs.data_start + (cluster - 2) * fat16_fs.sectors_per_cluster;

        for (uint8_t sector_idx = 0; sector_idx < fat16_fs.sectors_per_cluster && to_read > 0; ++sector_idx) {
            uint32_t sector_num = first_sector + sector_idx;
            if (fat16_read_sector(sector_num, (uint8_t*)buf + total_read)) return -2;

            size_t copy_size = fat16_fs.bytes_per_sector;
            if (copy_size > to_read) copy_size = to_read;

            total_read += copy_size;
            to_read -= copy_size;
        }

        uint16_t next_cluster = 0;
        if (fat16_read_fat_entry(cluster, &next_cluster)) return -3;
        if (next_cluster >= FAT16_CLUSTER_EOF || next_cluster == FAT16_CLUSTER_BAD) break;

        cluster = next_cluster;
    }

    if (read_count) *read_count = total_read;
    return 0;
}
