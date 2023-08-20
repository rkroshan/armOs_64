#ifndef __FS_H__
#define __FS_H__

#include "utils/memory.h"

struct BPB {
    uint8_t jump[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t fat_count;
    uint16_t root_entry_count;
    uint16_t sector_count;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sector_count;
    uint32_t large_sector_count;
    uint8_t drive_number;
    uint8_t flags;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t file_system[8];
} __attribute__((packed));

struct DirEntry {
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t create_ms;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t attr_index;
    uint16_t m_time;
    uint16_t m_date;
    uint16_t cluster_index;
    uint32_t file_size;
} __attribute__((packed));

/*file control block which actually hold the file information*/
struct FCB {
    char name[8];
    char ext[3];
    uint32_t cluster_index;
    uint32_t dir_index;
    uint32_t file_size;
    int count;
};

/*file descriptor struct holds the fcb which points to file and current position in file used by process*/
struct FileDesc {
    struct FCB *fcb;
    uint32_t position;
};

#define FS_BASE P2V(0x30000000)
#define ENTRY_EMPTY 0
#define ENTRY_DELETED 0xe5
#define MAX_FILENAME    8
#define MAX_FILE_EXT_NAME   3

struct Process;

/*do basic FAT16 filesystem bootup signature validation whether FS is available or not*/
void init_fs(void);
/*load the data from the file present at the path onto the VA addr, note it should be less than 2MB, there is no size param check at the moment*/
int load_file(char *path, uint64_t addr);
/*setup the fd and fcb for the process requesting the file*/
int open_file(struct Process *proc, char *path_name);
/*clear the fd and fcb for the file in the process struct*/
void close_file(struct Process *proc, int fd);
/*return the file size mentioned by fd*/
uint32_t get_file_size(struct Process *process, int fd);


#endif