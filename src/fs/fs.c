#include "fs.h"
#include "utils/print.h"
#include "utils/assert.h"
#include "utils/utils.h"
#include "process/process.h"
#include <stddef.h>

static struct FCB *fcb_table;
static struct FileDesc *file_desc_table;

static struct BPB* get_fs_bpb(void)
{
    uint32_t lba = *(uint32_t*)(FS_BASE + 0x1be + 8);

    return (struct BPB*)(FS_BASE + lba * 512);
}

static uint16_t *get_fat_table(void)
{
    struct BPB* p = get_fs_bpb();
    uint32_t offset = (uint32_t)p->reserved_sector_count * p->bytes_per_sector;

    return (uint16_t *)((uint8_t*)p + offset);
}

static uint16_t get_cluster_value(uint32_t cluster_index)
{
    uint16_t *fat_table = get_fat_table();

    return fat_table[cluster_index];
}

static uint32_t get_cluster_offset(uint32_t index)
{
    uint32_t res_size;
    uint32_t fat_size;
    uint32_t dir_size;

    ASSERT(index >= 2);

    struct BPB* p = get_fs_bpb();

    res_size = (uint32_t)p->reserved_sector_count * p->bytes_per_sector;
    fat_size = (uint32_t)p->fat_count * p->sectors_per_fat * p->bytes_per_sector;
    dir_size = (uint32_t)p->root_entry_count * sizeof(struct DirEntry);

    return res_size + fat_size + dir_size +
        (index - 2) * ((uint32_t)p->sectors_per_cluster * p->bytes_per_sector);
}

static uint32_t get_cluster_size(void)
{
    struct BPB* bpb = get_fs_bpb();

    return (uint32_t)bpb->bytes_per_sector * bpb->sectors_per_cluster;
}

static uint32_t get_root_directory_count(void)
{
    struct BPB* bpb = get_fs_bpb();

    return bpb->root_entry_count;
}

static struct DirEntry *get_root_directory(void)
{
    struct BPB *p; 
    uint32_t offset; 

    p = get_fs_bpb();
    offset = (p->reserved_sector_count + (uint32_t)p->fat_count * p->sectors_per_fat) * p->bytes_per_sector;

    return (struct DirEntry *)((uint8_t*)p + offset);
}

static bool is_file_name_equal(struct DirEntry *dir_entry, char *name, char *ext)
{
    bool status = false;

    if (memcmp(dir_entry->name, name, MAX_FILENAME) == 0 &&
        memcmp(dir_entry->ext, ext, MAX_FILE_EXT_NAME) == 0) {
        status = true;
    }

    return status;
}

static bool split_path(char *path, char *name, char *ext)
{
    int i;

    for (i = 0; i < MAX_FILENAME && path[i] != '.' && path[i] != '\0'; i++) {
        if (path[i] == '/') {
            return false;
        }

        name[i] = path[i];
    }

    if (path[i] == '.') {
        i++;
        
        for (int j = 0; j < MAX_FILE_EXT_NAME && path[i] != '\0'; i++, j++) {
            if (path[i] == '/') {
                return false;
            }

            ext[j] = path[i];
        }
    }

    if (path[i] != '\0') {        
        return false;
    }

    return true;
}

static uint32_t search_file(char *path)
{
    char name[8] = {"        "};
    char ext[3] =  {"   "};
    uint32_t root_entry_count;
    struct DirEntry *dir_entry; 

    bool status = split_path(path, name, ext); /*split filepath into name and extn*/

    if (status == true) {
        root_entry_count = get_root_directory_count();
        dir_entry = get_root_directory();
        
        for (uint32_t i = 0; i < root_entry_count; i++) { /*loop for all root directory count*/
            if (dir_entry[i].name[0] == ENTRY_EMPTY || dir_entry[i].name[0] == ENTRY_DELETED)
                continue; /*if dir entry name is empty or deleted continue*/

            if (dir_entry[i].attributes == 0xf) { /*it means directory supports long file name we don't so continue*/
                continue;
            }

            if (is_file_name_equal(&dir_entry[i], name, ext)) { /*check if filename matches then return the index*/
                return i;
            }
        }
    }

    return 0xffffffff;
}

/*actual read function*/
static uint32_t read_raw_data(uint32_t cluster_index, char *buffer, uint32_t size)
{
    struct BPB* bpb;
    char *data;
    uint32_t read_size = 0;
    uint32_t cluster_size; 
    uint32_t index; 
    
    bpb = get_fs_bpb(); /*get the bios param block*/
    cluster_size = get_cluster_size(); /* get the cluster size*/
    index = cluster_index;

    if (index < 2) { /*if cluster index is less than 2 return UINT32_MAX, it should not happen*/
        return 0xffffffff;
    }
    
    while (read_size < size) {
        data  = (char *)((uint64_t)bpb + get_cluster_offset(index)); /*get the offset where cluster data is present*/
        index = get_cluster_value(index); /*cluster data size at the offset, generally at max 512 bytes*/
        
        if (index >= 0xfff7) { /*it means last cluster index*/
            memcpy(buffer, data, size - read_size); /*copy the remaining data and return*/
            read_size += size - read_size;
            break;
        }

        memcpy(buffer, data, cluster_size); /*copy data upto cluster size*/

        buffer += cluster_size;
        read_size += cluster_size;
    }

    return read_size; /*return the total read_size*/
}

/*read file and copy data onto buffer */
static uint32_t read_file(uint32_t cluster_index, void *buffer, uint32_t size)
{
    return read_raw_data(cluster_index, buffer, size);
}

int load_file(char *path, uint64_t addr)
{
    uint32_t index;
    uint32_t file_size;
    uint32_t cluster_index;
    struct DirEntry *dir_entry;
    int ret = -1; /*Failure*/
    
    index = search_file(path); /*search for file from the path string given and return directory index, sub directories not supported at the moment*/

    if (index != 0xffffffff) { /*it means dir_entry index exist*/
        
        dir_entry = get_root_directory(); /*get the root directory*/
        file_size = dir_entry[index].file_size; /*get the file_size*/
        cluster_index = dir_entry[index].cluster_index; /*get the cluster index*/
        
        if (read_file(cluster_index, (void*)addr, file_size) == file_size) { /*read the file upto file_size should be < 2MB and copy onto VA provided*/
            ret = 0; /*success*/
        }
    }

    return ret;
}

/*check if fcb already exist or create one and inc the count*/
static uint32_t get_fcb(uint32_t index)
{
    struct DirEntry *dir_table;

    if (fcb_table[index].count == 0) {
        dir_table = get_root_directory();
        fcb_table[index].dir_index = index;
        fcb_table[index].file_size = dir_table[index].file_size;
        fcb_table[index].cluster_index = dir_table[index].cluster_index;
        memcpy(&fcb_table[index].name, &dir_table[index].name, 8);
        memcpy(&fcb_table[index].ext, &dir_table[index].ext, 3);
    }

    fcb_table[index].count++; /*increment the open count*/

    return index;
}

/*return the file size*/
uint32_t get_file_size(struct Process *process, int fd)
{
    return process->file[fd]->fcb->file_size;
}

int open_file(struct Process *proc, char *path_name)
{
    int fd = -1;
    int file_desc_index = -1;
    uint32_t entry_index;
    uint32_t fcb_index;

    for (int i = 0; i < 100; i++) { /*find free fd slot in process*/
        if (proc->file[i] == NULL) {
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        return -1;
    }

    for (int i = 0; i < PAGE_SIZE / sizeof(struct FileDesc); i++) {
        if (file_desc_table[i].fcb == NULL) { /*find free fd slot in fd table*/
            file_desc_index = i;
            break;
        }
    }

    if (file_desc_index == -1) {
        return -1;
    }

    entry_index = search_file(path_name); /*get the root directory index*/
    if (entry_index == 0xffffffff) {
        return -1;
    }

    fcb_index = get_fcb(entry_index); /*check if fcb already exist or create one and inc the count*/
    memset(&file_desc_table[file_desc_index], 0, sizeof(struct FileDesc)); /*clear fd table index struct*/
    file_desc_table[file_desc_index].fcb = &fcb_table[fcb_index]; /*assign the fd table index with the fcb we got*/
    proc->file[fd] = &file_desc_table[file_desc_index]; /*assignt eh fd table index addr to proc fd*/

    return fd;
}

/*dec the fcb open count*/
static void put_fcb(struct FCB *fcb)
{
    ASSERT(fcb->count > 0);
    fcb->count--;
}

void close_file(struct Process *proc, int fd)
{
    put_fcb(proc->file[fd]->fcb); /*dec the fcb open count*/

    proc->file[fd]->fcb = NULL; /*clear the fd of the process as well as the fd->fcb*/
    proc->file[fd] = NULL;
}

/*allocate one page memory for file control block*/
static bool init_fcb(void)
{
    fcb_table = (struct FCB*)kalloc();
    if (fcb_table == NULL) {
        return false;
    }

    memset(fcb_table, 0, PAGE_SIZE);

    return true;
}

/*allocate one page memory for file descriptors*/
static bool init_file_desc(void)
{
    file_desc_table = (struct FileDesc*)kalloc();
    if (file_desc_table == NULL) {
        return false;
    }

    memset(file_desc_table, 0, PAGE_SIZE);

    return true;
}

void init_fs(void)
{
    uint8_t *p = (uint8_t*)get_fs_bpb();
    
    if (p[0x1fe] != 0x55 || p[0x1ff] != 0xaa) {
        printk("invalid signature\n");
        ASSERT(0);
    }
    else {
        printk("file system is loaded\r\n");
    }

    /*once signature is verified initialize the fcb and fd blocks*/
    ASSERT(init_fcb());
    ASSERT(init_file_desc());
}

