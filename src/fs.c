#include "../include/fs.h"
#include "../include/cstr.h"
/* Block buffers (static = no heap) */
static u8 block_buffer[4096];
/* Block layout helpers */
#define SUPERBLOCK_BLOCK  0
#define INODE_BLOCK       1
#define BITMAP_BLOCK      2
#define DATA_START_BLOCK  3

/* Internal helpers */
/* ===============================
   Global filesystem disk
   =============================== */

static fs_disk* fs_active_disk = 0;

/* Initialize filesystem */
void fs_init(fs_disk* disk) {
    fs_active_disk = disk;
}

static void zero_block(void* buf, u32 size) {
    for (u32 i = 0; i < size; i++)
        ((u8*)buf)[i] = 0;
}

static void bitmap_set(u8* map, u32 index) {
    map[index / 8] |= (1 << (index % 8));
}

static int bitmap_test(u8* map, u32 index) {
    return (map[index / 8] >> (index % 8)) & 1;
}

static int alloc_block(fs_disk* disk) {
    disk->read_block(BITMAP_BLOCK, block_buffer);

    for (u32 i = DATA_START_BLOCK; i < disk->total_blocks; i++) {
        if (!bitmap_test(block_buffer, i)) {
            bitmap_set(block_buffer, i);
            disk->write_block(BITMAP_BLOCK, block_buffer);
            return i;
        }
    }
    return -1;
}

/* Load inode table */
static void load_inodes(fs_disk* disk, fs_inode* inodes) {
    disk->read_block(INODE_BLOCK, inodes);
}

static void save_inodes(fs_disk* disk, fs_inode* inodes) {
    disk->write_block(INODE_BLOCK, inodes);
}

static int find_inode(fs_inode* inodes, const str* name) {
    for (u32 i = 0; i < FS_MAX_INODES; i++) {
        if (!inodes[i].used) continue;

        str inode_name;
        inode_name.data = inodes[i].name;
        inode_name.len = str_len(inodes[i].name);

        if (str_cmp(&inode_name, name) == 0)
            return i;
    }
    return -1;
}

/* ===============================
   Public API
   =============================== */

void fs_format(fs_disk* disk) {
    fs_super sb;
    sb.magic        = FS_MAGIC;
    sb.block_size   = disk->block_size;
    sb.total_blocks = disk->total_blocks;
    sb.inode_count  = FS_MAX_INODES;
    sb.data_start   = DATA_START_BLOCK;

    zero_block(block_buffer, disk->block_size);
    *(fs_super*)block_buffer = sb;
    disk->write_block(SUPERBLOCK_BLOCK, block_buffer);

    /* Clear inode table */
    zero_block(block_buffer, disk->block_size);
    disk->write_block(INODE_BLOCK, block_buffer);

    /* Clear bitmap */
    zero_block(block_buffer, disk->block_size);
    disk->write_block(BITMAP_BLOCK, block_buffer);

    /* Reserve system blocks */
    bitmap_set(block_buffer, SUPERBLOCK_BLOCK);
    bitmap_set(block_buffer, INODE_BLOCK);
    bitmap_set(block_buffer, BITMAP_BLOCK);
    disk->write_block(BITMAP_BLOCK, block_buffer);
}

int fs_create_file(fs_disk* disk, const str* name) {
    fs_inode inodes[FS_MAX_INODES];
    load_inodes(disk, inodes);

    if (find_inode(inodes, name) >= 0)
        return -1;

    for (u32 i = 0; i < FS_MAX_INODES; i++) {
        if (!inodes[i].used) {
            inodes[i].used = 1;
            inodes[i].type = FS_TYPE_FILE;
            inodes[i].size = 0;

            for (u32 b = 0; b < FS_DIRECT_PTRS; b++)
                inodes[i].blocks[b] = 0;

            for (u32 c = 0; c < name->len && c < FS_NAME_MAX - 1; c++)
                inodes[i].name[c] = name->data[c];

            inodes[i].name[name->len] = '\0';

            save_inodes(disk, inodes);
            return 0;
        }
    }
    return -1;
}

int fs_write_file(fs_disk* disk, const str* name, const void* data, u32 size) {
    fs_inode inodes[FS_MAX_INODES];
    load_inodes(disk, inodes);

    int idx = find_inode(inodes, name);
    if (idx < 0) return -1;

    fs_inode* node = &inodes[idx];

    u32 written = 0;
    for (u32 i = 0; i < FS_DIRECT_PTRS && written < size; i++) {
        int blk = alloc_block(disk);
        if (blk < 0) break;

        node->blocks[i] = blk;

        u32 to_write = disk->block_size;
        if (size - written < to_write)
            to_write = size - written;

        zero_block(block_buffer, disk->block_size);
        for (u32 b = 0; b < to_write; b++)
            block_buffer[b] = ((const u8*)data)[written + b];

        disk->write_block(blk, block_buffer);
        written += to_write;
    }

    node->size = written;
    save_inodes(disk, inodes);
    return written;
}

int fs_read_file(fs_disk* disk, const str* name, void* out, u32 max_size) {
    fs_inode inodes[FS_MAX_INODES];
    load_inodes(disk, inodes);

    int idx = find_inode(inodes, name);
    if (idx < 0) return -1;

    fs_inode* node = &inodes[idx];
    u32 read = 0;

    for (u32 i = 0; i < FS_DIRECT_PTRS && read < node->size; i++) {
        if (!node->blocks[i]) break;

        disk->read_block(node->blocks[i], block_buffer);

        u32 to_read = disk->block_size;
        if (node->size - read < to_read)
            to_read = node->size - read;
        if (read + to_read > max_size)
            to_read = max_size - read;

        for (u32 b = 0; b < to_read; b++)
            ((u8*)out)[read + b] = block_buffer[b];

        read += to_read;
    }
    return read;
}
void fs_list_files(fs_list_cb callback) {
    if (!fs_active_disk)
        return;

    fs_inode inodes[FS_MAX_INODES];
    load_inodes(fs_active_disk, inodes);

    for (u32 i = 0; i < FS_MAX_INODES; i++) {
        if (!inodes[i].used)
            continue;

        str name;
        name.data = inodes[i].name;
        name.len  = str_len(inodes[i].name);

        callback(&name, inodes[i].size, inodes[i].type);
    }
}

