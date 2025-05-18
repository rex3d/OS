#include "fat32.h"
#include <string.h>

static int (*fat32_read_sector)(uint32_t lba, void* buf) = NULL;
static fat32_fs_t fat32_fs;
static fat32_dir_entry_t current_file_entry;
static uint32_t current_cluster;
static uint32_t current_cluster_pos; // pozycja w pliku

static inline uint16_t le16(const uint8_t* p) {
    return p[0] | (p[1] << 8);
}

static inline uint32_t le32(const uint8_t* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

int fat32_mount(int (*read_sector)(uint32_t lba, void* buf)) {
    if (!read_sector) return -1;
    fat32_read_sector = read_sector;

    uint8_t sector[512];
    if (fat32_read_sector(0, sector)) return -2; // błąd odczytu

    // Sprawdź czy to FAT32 (offset 82 i dalej to często oznaczenia FAT32)
    fat32_fs.bytes_per_sector = le16(&sector[11]);
    fat32_fs.sectors_per_cluster = sector[13];
    fat32_fs.reserved_sectors = le16(&sector[14]);
    fat32_fs.num_fats = sector[16];
    fat32_fs.sectors_per_fat = le32(&sector[36]);
    fat32_fs.root_cluster = le32(&sector[44]);
    fat32_fs.total_sectors = le32(&sector[32]);
    if (fat32_fs.total_sectors == 0) {
        fat32_fs.total_sectors = le16(&sector[19]);
    }

    fat32_fs.fat_start = fat32_fs.reserved_sectors;
    fat32_fs.data_start = fat32_fs.fat_start + fat32_fs.num_fats * fat32_fs.sectors_per_fat;

    // Sprawdź minimalne warunki
    if (fat32_fs.bytes_per_sector != 512) return -3; // obsługujemy tylko 512 bajtów sektor

    return 0;
}

static uint32_t fat32_first_sector_of_cluster(uint32_t cluster) {
    return fat32_fs.data_start + (cluster - 2) * fat32_fs.sectors_per_cluster;
}

static uint32_t fat32_get_fat_entry(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector_num = fat32_fs.fat_start + (fat_offset / fat32_fs.bytes_per_sector);
    uint32_t offset_in_sector = fat_offset % fat32_fs.bytes_per_sector;

    uint8_t buf[512];
    if (fat32_read_sector(fat_sector_num, buf)) return 0x0FFFFFFF; // błąd - traktuj jako koniec

    uint32_t val = le32(&buf[offset_in_sector]) & 0x0FFFFFFF;
    return val;
}

static int fat32_compare_filename(const char* input, const uint8_t* entry_name) {
    // Sprawdź czy input to dokładnie 8.3 bez kropek i null-terminatora, a entry_name to 11 znaków
    // input ma format "FILENAMEEXT" bez kropek, np "README   TXT"
    for (int i = 0; i < FAT32_MAX_FILENAME; i++) {
        char c1 = input[i];
        char c2 = entry_name[i];
        if (c1 >= 'a' && c1 <= 'z') c1 -= 32; // uppercase
        if (c1 != c2) return 0;
    }
    return 1;
}

int fat32_open(const char* filename, fat32_dir_entry_t* entry) {
    if (!fat32_read_sector) return -1;

    uint8_t sector[512];
    uint32_t cluster = fat32_fs.root_cluster;

    while (1) {
        uint32_t first_sector = fat32_first_sector_of_cluster(cluster);

        for (uint8_t sector_offset = 0; sector_offset < fat32_fs.sectors_per_cluster; sector_offset++) {
            if (fat32_read_sector(first_sector + sector_offset, sector)) return -2;

            for (int i = 0; i < 512; i += 32) {
                fat32_dir_entry_t* dir = (fat32_dir_entry_t*)&sector[i];

                if (dir->name[0] == 0x00) return -3; // koniec katalogu
                if (dir->name[0] == 0xE5) continue; // usunięty plik

                if (dir->attr == 0x0F) continue; // pomiń wpis LFN

                if (fat32_compare_filename(filename, dir->name)) {
                    if (entry) memcpy(entry, dir, sizeof(fat32_dir_entry_t));

                    // ustaw aktualny plik i pozycję
                    current_file_entry = *dir;
                    current_cluster = (dir->first_cluster_high << 16) | dir->first_cluster_low;
                    current_cluster_pos = 0;

                    return 0;
                }
            }
        }
        // Idź do następnego klastra katalogu
        cluster = fat32_get_fat_entry(cluster);
        if (cluster >= 0x0FFFFFF8) break; // koniec
    }

    return -4; // plik nie znaleziony
}

int fat32_read(const fat32_dir_entry_t* entry, void* buf, size_t count, size_t* read_count) {
    if (!fat32_read_sector) return -1;
    if (!entry || !buf || !read_count) return -2;

    uint8_t sector[512];
    size_t bytes_read = 0;

    uint32_t file_size = entry->size;
    uint32_t cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;
    uint32_t pos = 0;

    while (bytes_read < count && pos < file_size && cluster < 0x0FFFFFF8) {
        uint32_t first_sector = fat32_first_sector_of_cluster(cluster);

        for (uint8_t sector_offset = 0; sector_offset < fat32_fs.sectors_per_cluster; sector_offset++) {
            if (bytes_read >= count || pos >= file_size) break;

            if (fat32_read_sector(first_sector + sector_offset, sector)) return -3;

            uint32_t sector_pos = 0;
            while (sector_pos < fat32_fs.bytes_per_sector && bytes_read < count && pos < file_size) {
                ((uint8_t*)buf)[bytes_read++] = sector[sector_pos++];
                pos++;
            }
        }

        cluster = fat32_get_fat_entry(cluster);
    }

    *read_count = bytes_read;
    return 0;
}
