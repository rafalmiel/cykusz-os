#ifndef FS_H
#define FS_H

#include <core/common.h>

#define FS_FILE		0x01
#define FS_DIRECTORY	0x02
#define FS_CHARDEVICE	0x03
#define FS_BLOCKDEVICE	0x04;
#define FS_PIPE		0x05
#define FS_SYMLINK	0x06
#define FS_MOUNTPOINT	0x08

struct fs_node;

struct dirent {
	char name[128];
	u32 ino;
};

typedef u32 (*fs_read_t)(struct fs_node*, u32, u32, u8*);
typedef u32 (*fs_write_t)(struct fs_node*, u32, u32, u8*);
typedef void (*fs_open_t)(struct fs_node*, u8, u8);
typedef void (*fs_close_t)(struct fs_node*);
typedef struct dirent *(*fs_readdir_t)(struct fs_node*, u32);
typedef struct fs_node *(*fs_finddir_t)(struct fs_node*, const char *name);

typedef struct fs_node {
	char name[128];
	u32 mask;
	u32 uid;
	u32 gid;
	u32 flags;
	u32 inode;
	u32 length;
	u32 impl;

	fs_read_t read;
	fs_write_t write;
	fs_open_t open;
	fs_close_t close;
	fs_readdir_t readdir;
	fs_finddir_t finddir;

	struct fs_node *ptr;
} fs_node_t;

u32 fs_read(fs_node_t *node, u32 offset, u32 size, u8 *buffer);
u32 fs_write(fs_node_t *node, u32 offset, u32 size, u8 *buffer);
void fs_open(fs_node_t *node, u8 read, u8 write);
void fs_close(fs_node_t *node);
struct dirent *fs_readdir(fs_node_t *node, u32 index);
fs_node_t *fs_finddir(fs_node_t *node, const char *name);

#endif // FS_H
