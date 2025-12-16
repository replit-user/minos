#ifndef FS_H
#define FS_H

#include "cstr.h"

/* Fixed-size types (no stdint.h) */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

/* Disk interface (YOU implement these) */
typedef struct {
    u32 block_size;
    u32 total_blocks;

    void (*read_block)(u32 block, void* buffer);
    void (*write_block)(u32 block, const void* buffer);
} fs_disk;

/* Filesystem constants */
#define FS_MAGIC        0x53465331  /* "SFS1" */
#define FS_MAX_INODES   128
#define FS_NAME_MAX     32
#define FS_DIRECT_PTRS  8

/* File types */
#define FS_TYPE_FILE 1
#define FS_TYPE_DIR  2

/* Inode structure (on disk) */
typedef struct {
    u8  used;
    u8  type;
    u16 reserved;

    u32 size;
    u32 blocks[FS_DIRECT_PTRS];

    char name[FS_NAME_MAX];
} fs_inode;

/* Superblock */
typedef struct {
    u32 magic;
    u32 block_size;
    u32 total_blocks;
    u32 inode_count;
    u32 data_start;
} fs_super;

/* Public API */
void fs_format(fs_disk* disk);
int  fs_create_file(fs_disk* disk, const str* name);
int  fs_write_file(fs_disk* disk, const str* name, const void* data, u32 size);
int  fs_read_file(fs_disk* disk, const str* name, void* out, u32 max_size);
/* Callback-based file listing */
typedef void (*fs_list_cb)(const str* name, u32 size, u8 type);
/* Initialize filesystem with a disk (call once at boot) */
void fs_init(fs_disk* disk);

/* List files using global disk */
void fs_list_files(fs_list_cb callback);

#endif
