#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maksymalna długość nazwy pliku FAT16 (8+3 bez kropki)
#define FAT16_MAX_FILENAME   11
#define FAT16_MAX_PATH       260

// Atrybuty pliku FAT16 (bitowe flagi)
#define FAT16_ATTR_READ_ONLY  0x01
#define FAT16_ATTR_HIDDEN     0x02
#define FAT16_ATTR_SYSTEM     0x04
#define FAT16_ATTR_VOLUME_ID  0x08
#define FAT16_ATTR_DIRECTORY  0x10
#define FAT16_ATTR_ARCHIVE    0x20
#define FAT16_ATTR_LFN        (FAT16_ATTR_READ_ONLY | FAT16_ATTR_HIDDEN | FAT16_ATTR_SYSTEM | FAT16_ATTR_VOLUME_ID)

// Kody błędów FAT16 (enum)
typedef enum {
    FAT16_OK = 0,
    FAT16_ERR_IO,
    FAT16_ERR_NO_FS,
    FAT16_ERR_INVALID_ARG,
    FAT16_ERR_NOT_FOUND,
    FAT16_ERR_NO_SPACE,
    FAT16_ERR_CORRUPTED,
    FAT16_ERR_UNSUPPORTED,
    FAT16_ERR_EOF,
    FAT16_ERR_BUSY,
    FAT16_ERR_UNKNOWN
} fat16_result_t;

// Wpis katalogowy FAT16 (8.3 format) – bez żadnego pierdololo, packed
typedef struct __attribute__((packed)) {
    uint8_t  name[FAT16_MAX_FILENAME];    // 8+3 nazwa bez kropki, spacje dla paddingu
    uint8_t  attr;                        // Atrybuty pliku (bitmask)
    uint8_t  nt_reserved;                 // Reserved for NT, kurwa, nie ruszać
    uint8_t  creation_time_tenths;       // 1/10 sekundy przy tworzeniu
    uint16_t creation_time;               // Czas utworzenia (DOS format)
    uint16_t creation_date;               // Data utworzenia
    uint16_t last_access_date;            // Data ostatniego dostępu
    uint16_t first_cluster_high;          // High word klastra startowego (FAT32, zawsze 0 w FAT16)
    uint16_t write_time;                  // Czas ostatniego zapisu
    uint16_t write_date;                  // Data ostatniego zapisu
    uint16_t first_cluster_low;           // Low word klastra startowego
    uint32_t size;                        // Rozmiar pliku w bajtach
} fat16_dir_entry_t;

// Opis systemu plików FAT16 - jak jebnięty bajt na mega maszynie
typedef struct {
    uint16_t bytes_per_sector;           // Bajtów na sektor (zazwyczaj 512)
    uint8_t  sectors_per_cluster;        // Sektorów na klaster
    uint16_t reserved_sectors;           // Zarezerwowane sektory przed FAT
    uint8_t  num_fats;                   // Liczba FATów (zwykle 2)
    uint16_t root_entries;               // Ilość wpisów w katalogu root
    uint16_t total_sectors_16;           // Total sectors (jeśli <65535)
    uint8_t  media_type;                 // Typ nośnika
    uint16_t sectors_per_fat;            // Ilość sektorów na FAT
    uint16_t sectors_per_track;          // Geometryczne dane dysku
    uint16_t num_heads;                  // Geometryczne dane dysku
    uint32_t hidden_sectors;             // Ukryte sektory (dla partycji)
    uint32_t total_sectors_32;           // Total sectors (jeśli >65535)
    uint32_t fat_start;                  // LBA start FAT
    uint32_t root_start;                 // LBA start katalogu root
    uint32_t data_start;                 // LBA start danych (klastry)
    uint32_t root_sectors;               // Liczba sektorów katalogu root
    uint32_t cluster_count;              // Ilość klastrów (cały obszar danych)
    uint32_t last_allocated_cluster;    // Ostatni alokowany klaster (cache)
    int (*read_sector)(uint32_t lba, void* buf);
    int (*write_sector)(uint32_t lba, const void* buf);
    void* user_data;                     // Dla własnych śmieci w implementacji
} fat16_fs_t;

// Uchwyt pliku FAT16 (otwarty plik)
typedef struct {
    fat16_dir_entry_t entry;             // Wpis katalogowy
    uint32_t current_cluster;            // Aktualny klaster
    uint32_t file_offset;                // Pozycja w pliku (w bajtach)
    bool     eof;                        // Flaga EOF
    fat16_fs_t* fs;                     // Wskaźnik do systemu plików
} fat16_file_t;

// Uchwyt katalogu FAT16 (otwarty katalog)
typedef struct {
    fat16_dir_entry_t entry;             // Wpis katalogowy (dla katalogu)
    uint32_t current_entry;              // Aktualny indeks wpisu
    uint32_t total_entries;              // Całkowita liczba wpisów
    fat16_fs_t* fs;                     // Wskaźnik do systemu plików
    uint8_t* sector_buf;                 // Bufor na sektor katalogu
} fat16_dir_t;

// Montowanie i odmontowanie FS
fat16_result_t fat16_mount(fat16_fs_t* fs,
                           int (*read_sector)(uint32_t lba, void* buf),
                           int (*write_sector)(uint32_t lba, const void* buf),
                           void* user_data) __attribute__((nonnull(1,2)));

fat16_result_t fat16_unmount(fat16_fs_t* fs) __attribute__((nonnull(1)));

// Operacje na plikach
fat16_result_t fat16_open(fat16_fs_t* fs, const char* path, fat16_file_t* file) __attribute__((nonnull(1,2,3)));
fat16_result_t fat16_create(fat16_fs_t* fs, const char* path, fat16_file_t* file) __attribute__((nonnull(1,2,3)));
fat16_result_t fat16_read(fat16_file_t* file, void* buf, size_t count, size_t* read_count) __attribute__((nonnull(1,2)));
fat16_result_t fat16_write(fat16_file_t* file, const void* buf, size_t count, size_t* written_count) __attribute__((nonnull(1,2)));
fat16_result_t fat16_seek(fat16_file_t* file, uint32_t offset) __attribute__((nonnull(1)));
fat16_result_t fat16_close(fat16_file_t* file) __attribute__((nonnull(1)));
fat16_result_t fat16_remove(fat16_fs_t* fs, const char* path) __attribute__((nonnull(1,2)));

// Operacje na katalogach
fat16_result_t fat16_opendir(fat16_fs_t* fs, const char* path, fat16_dir_t* dir) __attribute__((nonnull(1,2,3)));
fat16_result_t fat16_readdir(fat16_dir_t* dir, fat16_dir_entry_t* entry) __attribute__((nonnull(1,2)));
fat16_result_t fat16_closedir(fat16_dir_t* dir) __attribute__((nonnull(1)));
fat16_result_t fat16_mkdir(fat16_fs_t* fs, const char* path) __attribute__((nonnull(1,2)));

// Narzędzia i helpery

// Sprawdza czy nazwa pliku jest prawidłowa wg FAT16 (8.3 + rozszerzenia)
bool fat16_is_valid_filename(const char* filename) __attribute__((nonnull(1)));

// Konwertuje zwykły path/nazwę do formatu FAT16 8.3 w buforze
void fat16_format_filename(const char* input, uint8_t output[FAT16_MAX_FILENAME]) __attribute__((nonnull(1,2)));

// Sprawdza czy wpis to katalog
static inline bool fat16_is_dir(const fat16_dir_entry_t* entry) {
    return (entry->attr & FAT16_ATTR_DIRECTORY) != 0;
}

// Sprawdza czy wpis to wolny (usunięty lub pusty)
static inline bool fat16_is_entry_free(const fat16_dir_entry_t* entry) {
    return entry->name[0] == 0x00 || entry->name[0] == 0xE5;
}

#ifdef __cplusplus
}
#endif

#endif // FAT16_H
