#include "fat.h"
#include <string.h>
#include <stdio.h>

// Makra do little-endian
#define LE16(x) ((uint16_t)((x)[0] | ((x)[1] << 8)))
#define LE32(x) ((uint32_t)((x)[0] | ((x)[1] << 8) | ((x)[2] << 16) | ((x)[3] << 24)))

// Pomocnicze - BPB i FAT32 struktury
typedef struct {
    uint8_t  jump[3];
    uint8_t  oem[8];
    uint8_t  bpb[25]; // od offsetu 11
    uint8_t  ext[54]; // rozszerzona część dla FAT32
} __attribute__((packed)) fat_boot_sector_t;

static uint16_t fat_get_fat12_entry(uint8_t* fat_buf, uint32_t cluster) {
    uint32_t offset = (cluster * 3) / 2;
    uint16_t value;
    if (cluster & 1) {
        value = ((fat_buf[offset] >> 4) | (fat_buf[offset + 1] << 4)) & 0x0FFF;
    } else {
        value = (fat_buf[offset] | ((fat_buf[offset + 1] & 0x0F) << 8)) & 0x0FFF;
    }
    return value;
}

static uint32_t fat_get_fat16_entry(uint8_t* fat_buf, uint32_t cluster) {
    uint32_t offset = cluster * 2;
    return fat_buf[offset] | (fat_buf[offset + 1] << 8);
}

static uint32_t fat_get_fat32_entry(uint8_t* fat_buf, uint32_t cluster) {
    uint32_t offset = cluster * 4;
    uint32_t val = fat_buf[offset] | (fat_buf[offset + 1] << 8) | (fat_buf[offset + 2] << 16) | (fat_buf[offset + 3] << 24);
    return val & 0x0FFFFFFF; // 28 bitów ważne
}

static int fat_read_fat_table(fat_fs_t* fs, uint8_t* fat_buf) {
    uint32_t fat_size_sectors = (fs->type == FAT_TYPE_32) ? fs->sectors_per_fat_32 : fs->sectors_per_fat_16;
    for (uint32_t i = 0; i < fat_size_sectors; i++) {
        if (fs->read_sector(fs->fat_start + i, fat_buf + i * fs->bytes_per_sector)) return -1;
    }
    return 0;
}

static uint32_t fat_cluster_to_lba(fat_fs_t* fs, uint32_t cluster, uint8_t sector_in_cluster) {
    return fs->data_start + (cluster - 2) * fs->sectors_per_cluster + sector_in_cluster;
}

// Inicjalizacja FAT i rozpoznanie typu
int fat_mount(fat_fs_t* fs, int (*read_sector)(uint32_t lba, void* buf)) {
    if (!fs || !read_sector) return -1;

    uint8_t sector[512];
    if (read_sector(0, sector)) return -1;

    fs->bytes_per_sector    = LE16(&sector[11]);
    fs->sectors_per_cluster = sector[13];
    fs->reserved_sectors    = LE16(&sector[14]);
    fs->num_fats            = sector[16];
    fs->root_entries        = LE16(&sector[17]);
    uint16_t total_sectors_16 = LE16(&sector[19]);
    fs->media_type          = sector[21];
    fs->sectors_per_fat_16  = LE16(&sector[22]);
    uint32_t total_sectors_32 = LE32(&sector[32]);
    uint32_t sectors_per_fat_32 = LE32(&sector[36]);

    fs->read_sector = read_sector;

    // Liczymy total_sectors
    if (total_sectors_16 != 0) {
        fs->total_sectors = total_sectors_16;
    } else {
        fs->total_sectors = total_sectors_32;
    }

    // Określamy sektory per FAT
    if (fs->sectors_per_fat_16 != 0) {
        fs->sectors_per_fat_16 = fs->sectors_per_fat_16;
    } else {
        fs->sectors_per_fat_16 = 0;
    }

    if (sectors_per_fat_32 != 0) {
        fs->sectors_per_fat_32 = sectors_per_fat_32;
    } else {
        fs->sectors_per_fat_32 = 0;
    }

    fs->fat_start = fs->reserved_sectors;

    // FAT32 root dir is a cluster chain, FAT12/16 root dir fixed
    if (fs->sectors_per_fat_32 != 0) {
        fs->type = FAT_TYPE_32;
        fs->root_start = fat_cluster_to_lba(fs, LE32(&sector[44]), 0); // cluster start root dir from BS
    } else if (fs->root_entries != 0) {
        uint32_t root_dir_sectors = ((fs->root_entries * 32) + (fs->bytes_per_sector - 1)) / fs->bytes_per_sector;
        fs->type = (fs->total_sectors / fs->sectors_per_fat_16) < 4085 ? FAT_TYPE_12 : FAT_TYPE_16;
        fs->root_start = fs->fat_start + fs->num_fats * (fs->sectors_per_fat_16);
        fs->data_start = fs->root_start + root_dir_sectors;
    } else {
        // Nie wiadomo co jest, ustawiamy NONE
        fs->type = FAT_TYPE_NONE;
    }

    if (fs->type != FAT_TYPE_32) {
        uint32_t root_dir_sectors = ((fs->root_entries * 32) + (fs->bytes_per_sector - 1)) / fs->bytes_per_sector;
        fs->data_start = fs->root_start + root_dir_sectors;
    } else {
        // FAT32 data start już ustawione
        // Możemy obliczyć total_clusters
        fs->total_clusters = (fs->total_sectors - fs->data_start) / fs->sectors_per_cluster;
    }

    return 0;
}

// Szuka pliku w root directory FAT12/16 lub FAT32 (prostota: szuka tylko w root!)
int fat_open(fat_fs_t* fs, const char* filename, fat_dir_entry_t* entry) {
    if (!fs || !filename || !entry) return -1;

    uint8_t sector[512];
    uint32_t root_dir_sectors;

    if (fs->type == FAT_TYPE_32) {
        // FAT32 root directory jest łańcuchem klastrów - tu trzeba zrobić bardziej zaawansowane, uprościmy i zwracamy -1
        // Możesz rozbudować jak chcesz, ale tu zostawiam na później
        return -1;
    } else {
        // FAT12/16 - root directory jest w sektorach bezpośrednich
        root_dir_sectors = ((fs->root_entries * 32) + (fs->bytes_per_sector - 1)) / fs->bytes_per_sector;
        for (uint32_t i = 0; i < root_dir_sectors; ++i) {
            if (fs->read_sector(fs->root_start + i, sector)) return -1;
            for (int off = 0; off < fs->bytes_per_sector; off += 32) {
                uint8_t first_byte = sector[off];
                if (first_byte == 0x00) {
                    // koniec katalogu
                    return -1;
                }
                if ((first_byte == 0xE5) || (sector[off + 11] & 0x08)) {
                    // usunięty lub volume label - pomijamy
                    continue;
                }

                if (memcmp(&sector[off], filename, FAT_MAX_FILENAME) == 0) {
                    memcpy(entry, &sector[off], sizeof(fat_dir_entry_t));
                    return 0;
                }
            }
        }
    }

    return -1;
}

// Czyta plik - tu trzeba jeszcze dopieścić, w wersji podstawowej tylko 1 sektor klastrów itp.
int fat_read(fat_fs_t* fs, const fat_dir_entry_t* entry, void* buf, size_t count, size_t* read_count) {
    if (!fs || !entry || !buf || !read_count) return -1;

    // Tylko prosta implementacja FAT12/16, FAT32 pomijamy
    if (fs->type == FAT_TYPE_32) {
        return -1;
    }

    uint32_t first_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
    if (first_cluster == 0) first_cluster = 0; // Root directory?

    uint32_t bytes_per_cluster = fs->bytes_per_sector * fs->sectors_per_cluster;
    uint32_t bytes_to_read = count < entry->size ? count : entry->size;

    uint8_t sector[512];
    size_t total_read = 0;

    // Upraszczamy: czytamy tylko 1 klaster
    for (uint8_t sector_idx = 0; sector_idx < fs->sectors_per_cluster; ++sector_idx) {
        if (total_read >= bytes_to_read) break;
        uint32_t lba = fat_cluster_to_lba(fs, first_cluster, sector_idx);
        if (fs->read_sector(lba, sector)) return -1;

        size_t bytes_left = bytes_to_read - total_read;
        size_t to_copy = bytes_left < fs->bytes_per_sector ? bytes_left : fs->bytes_per_sector;
        memcpy((uint8_t*)buf + total_read, sector, to_copy);
        total_read += to_copy;
    }

    *read_count = total_read;
    return 0;
}
