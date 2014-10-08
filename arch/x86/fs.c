#include "fs.h"

u32 fs_read(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
	if (node->read != 0) {
		return node->read(node, offset, size, buffer);
	} else {
		return 0;
	}
}

u32 fs_write(fs_node_t *node, u32 offset, u32 size, u8 *buffer)
{
	if (node->write != 0) {
		return node->write(node, offset, size, buffer);
	} else {
		return 0;
	}
}

void fs_open(fs_node_t *node, u8 read, u8 write)
{
	if (node->open != 0) {
		node->open(node, read, write);
	}
}

void fs_close(fs_node_t *node)
{
	if (node->close != 0) {
		node->close(node);
	}
}

struct dirent *fs_readdir(fs_node_t *node, u32 index)
{
	if (node->readdir != 0
	    && ((node->flags & FS_DIRECTORY) == FS_DIRECTORY)) {
		return node->readdir(node, index);
	} else {
		return 0;
	}
}

fs_node_t *fs_finddir(fs_node_t *node, const char *name)
{
	if (node->finddir != 0
	    && ((node->flags & FS_DIRECTORY) == FS_DIRECTORY)) {
		return node->finddir(node, name);
	} else {
		return 0;
	}
}
