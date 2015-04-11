#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define BFS_MAGIC 0x1BADFACE
#define BLOCK_SIZE 512
#define BFS_ATTR_FILE 1
#define BFS_ATTR_DIR 2

typedef struct bfs_superblock {
	int32_t magic;		// Magic number (0x1BADFACE)
	int32_t start;		// Start of data block (in bytes)
	int32_t size;		// Size of filesystem
	int32_t sanity[4];	// Sanity words used to recover filesystem
} bfs_superblock_t;

typedef struct bfs_inode {
	int32_t inode;		// Inode number
	int32_t first_block;	// First block of file
	int32_t last_block;	// Last block of file
	int32_t offset_to_eof;	// Disk offset to end of file
	int32_t attributes;	// File attribs (1 = regular, 2 = directory)
	int32_t mode;		// File mode, rwxrwxrwx (only low 9 bits used)
	int32_t uid;		// File owner - user id
	int32_t gid;		// File owner - group id
	int32_t nlinks;		// Hard link count
	int32_t atime;		// Access time
	int32_t mtime;		// Modify time
	int32_t ctime;		// Create time
	int32_t spare[4];	// Unused, should be zero
} bfs_inode_t;

typedef struct bfs_dirent {
	int16_t inode;		// File inode number
	char filename[14];	// File name
} bfs_dirent_t;

typedef struct bfs_file {
	FILE *file;
	int numfiles;
	int inode_cnt;
	int inode_size;
	int inode_block_cnt;
	int dirent_start_block;
	int dirent_cnt;
	int dirent_size;
	int dirent_block_cnt;

	int data_start_block;

	int cur_inode;
	int cur_inode_cnt;
	int cur_dirent_cnt;
	int cur_datablock_cnt;

	bfs_superblock_t superblock;
	bfs_inode_t dirent_inode;
} bfs_file_t;

static uint32_t align(uint32_t value, uint32_t to)
{
	if (value & (to - 1)) {
		value &= ~(to - 1);
		value += to;
	}

	return value;
}

static uint32_t align_BS(uint32_t addr)
{
	return align(addr, BLOCK_SIZE);
}

off_t fsize(const char *filename) {
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;

	return -1;
}

static void write_at(bfs_file_t *bfs, void *data, uint32_t size, uint32_t off)
{
	fseek(bfs->file, off, SEEK_SET);
	fwrite(data, size, 1, bfs->file);
}

static void add_dirent(bfs_file_t *bfs, int inode_num, const char *name)
{
	bfs_dirent_t dirent;
	memset(&dirent, 0, sizeof(dirent));
	strcpy(dirent.filename, name);
	dirent.inode = inode_num;

	uint32_t offset = bfs->dirent_start_block * BLOCK_SIZE
			+ bfs->cur_dirent_cnt * sizeof(bfs_dirent_t);

	write_at(bfs, &dirent, sizeof(bfs_dirent_t), offset);

	bfs->cur_dirent_cnt++;
}

static void prepare_standard_dirents(bfs_file_t *bfs)
{
	bfs->dirent_inode.inode = bfs->cur_inode++;
	bfs->dirent_inode.first_block = bfs->dirent_start_block;
	bfs->dirent_inode.last_block = bfs->dirent_start_block
			+ bfs->dirent_block_cnt - 1;
	bfs->dirent_inode.offset_to_eof = bfs->dirent_start_block * BLOCK_SIZE
			+ bfs->dirent_cnt * sizeof(bfs_dirent_t);
	bfs->dirent_inode.attributes = BFS_ATTR_DIR;
	bfs->dirent_inode.mode = 0b111111111;
	bfs->dirent_inode.uid = 0;
	bfs->dirent_inode.gid = 0;
	bfs->dirent_inode.nlinks = 0;
	bfs->dirent_inode.atime = 0;
	bfs->dirent_inode.mtime = 0;
	bfs->dirent_inode.ctime = 0;
	memset(&bfs->dirent_inode.spare, 0, sizeof(int32_t) * 4);

	add_dirent(bfs, 2, ".");
	add_dirent(bfs, 2, "..");
}

static int get_block_count(int size)
{
	return align_BS(size) / BLOCK_SIZE;
}

static bfs_file_t *open_bfs(int argc, char *argv[])
{
	bfs_file_t *bfs = malloc(sizeof(bfs_file_t));

	bfs->numfiles = argc - 2;

	bfs->inode_cnt = bfs->numfiles + 1;
	bfs->inode_size = bfs->inode_cnt * sizeof(bfs_inode_t);
	bfs->inode_block_cnt = get_block_count(bfs->inode_size);

	printf("Inode block count: %d\n", bfs->inode_block_cnt);

	bfs->dirent_start_block = bfs->inode_block_cnt + 1;
	bfs->dirent_cnt = bfs->numfiles + 2;
	bfs->dirent_size = bfs->dirent_cnt * sizeof(bfs_dirent_t);
	bfs->dirent_block_cnt = get_block_count(bfs->dirent_size);

	bfs->data_start_block = bfs->dirent_start_block + bfs->dirent_block_cnt;
	printf("Data start block: %d\n", bfs->data_start_block);

	bfs->cur_inode = 2;
	bfs->cur_inode_cnt = 1;
	bfs->cur_dirent_cnt = 0;
	bfs->cur_datablock_cnt = 0;

	bfs->superblock.magic = BFS_MAGIC;
	bfs->superblock.start = bfs->dirent_start_block * BLOCK_SIZE;
	bfs->superblock.size = bfs->superblock.start
			+ bfs->dirent_block_cnt * BLOCK_SIZE;
	memset(bfs->superblock.sanity, 0xFFFFFFFF, sizeof(int32_t) * 4);

	printf("Opening %s\n", argv[1]);
	bfs->file = fopen(argv[1], "w+");

	if (bfs->file == 0) {
		fprintf(stderr, "Failed to open the file\n");
		return 0;
	}

	prepare_standard_dirents(bfs);

	return bfs;
}

static void close_bfs(bfs_file_t *bfs)
{
	write_at(bfs, &bfs->superblock,
		 sizeof(bfs->superblock), 0);

	write_at(bfs, &bfs->dirent_inode,
		 sizeof(bfs->dirent_inode), BLOCK_SIZE);

	ftruncate(fileno(bfs->file), bfs->superblock.size);

	fclose(bfs->file);

	free(bfs);
}

static int32_t write_file_inode(bfs_file_t *bfs, off_t size)
{
	off_t al_size = align_BS(size);

	int block_count = get_block_count(size);

	bfs_inode_t inode;

	inode.inode = bfs->cur_inode++;
	inode.first_block = bfs->data_start_block + bfs->cur_datablock_cnt;
	inode.last_block = inode.first_block + block_count - 1;
	inode.offset_to_eof = inode.first_block * BLOCK_SIZE + size;
	inode.attributes = BFS_ATTR_FILE;
	inode.mode = 0b111111111;
	inode.uid = inode.gid = 0;
	inode.nlinks = 1;
	inode.atime = inode.mtime = inode.ctime = 0;
	memset(&inode.spare, 0, sizeof(int32_t) * 4);

	write_at(bfs, &inode, sizeof(bfs_inode_t),
		 BLOCK_SIZE + bfs->cur_inode_cnt * sizeof(bfs_inode_t));

	/* Update metadata values */
	bfs->cur_datablock_cnt += block_count;
	bfs->cur_inode_cnt++;
	bfs->superblock.size += al_size;

	return inode.inode;
}

static void write_file_contents(bfs_file_t *bfs, void *data, off_t size,
				const char *filename)
{
	write_at(bfs, data, size,
		 bfs->data_start_block * BLOCK_SIZE
		 + BLOCK_SIZE * bfs->cur_datablock_cnt);

	printf("Written %d data at offset %d\n",
	       size,
	       bfs->data_start_block * BLOCK_SIZE
	       + BLOCK_SIZE * bfs->cur_datablock_cnt);

	int32_t inode = write_file_inode(bfs, size);

	add_dirent(bfs, inode, filename);
}

static void write_file(bfs_file_t *bfs, const char *filename)
{
	printf("Writing %s\n", filename);

	off_t size = fsize(filename);
	FILE *file = fopen(filename, "r");
	char *buf = malloc(size);
	fread(buf, 1, size, file);

	write_file_contents(bfs, buf, size, filename);

	free(buf);
}

int main(int argc, char *argv[])
{
	int cnt;

	if (argc < 2) {
		printf("Usage: %s image_name [file1, file2, ...] \n", argv[0]);
		return 0;
	}

	bfs_file_t *bfs = open_bfs(argc, argv);

	if (!bfs) {
		return 1;
	}

	for (cnt = 2; cnt < argc; ++cnt) {
		write_file(bfs, argv[cnt]);
	}

	close_bfs(bfs);

	return 0;
}
