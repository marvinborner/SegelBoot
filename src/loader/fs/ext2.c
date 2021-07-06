// MIT License, Copyright (c) 2021 Marvin Borner

#include <dev.h>
#include <fs/ext2.h>
#include <lib.h>
#include <pnc.h>

static void ext2_buffer_read(u32 block, void *buf, struct dev *dev)
{
	dev->read(buf, block * SECTOR_COUNT, SECTOR_COUNT, dev);
}

static void ext2_get_superblock(struct ext2_superblock *buf, struct dev *dev)
{
	u8 data[BLOCK_SIZE] = { 0 };
	ext2_buffer_read(EXT2_SUPER, data, dev);
	memcpy(buf, data, sizeof(*buf));

	assert(buf->magic == EXT2_MAGIC);
}

static struct ext2_inode *ext2_get_inode(u32 i, struct ext2_inode *in_buf, struct dev *dev)
{
	struct ext2_superblock sb = { 0 };
	ext2_get_superblock(&sb, dev);

	u8 data[BLOCK_SIZE] = { 0 };
	ext2_buffer_read(EXT2_SUPER + 1, data, dev);
	struct ext2_bgd *bgd = (struct ext2_bgd *)data;

	u32 block_group = (i - 1) / sb.inodes_per_group;
	u32 index = (i - 1) % sb.inodes_per_group;
	u32 block = (index * EXT2_INODE_SIZE) / BLOCK_SIZE;
	bgd += block_group;

	u8 buf[BLOCK_SIZE] = { 0 };
	ext2_buffer_read(bgd->inode_table + block, buf, dev);
	struct ext2_inode *in =
		(struct ext2_inode *)((u32)buf +
				      (index % (BLOCK_SIZE / EXT2_INODE_SIZE)) * EXT2_INODE_SIZE);

	memcpy(in_buf, in, sizeof(*in_buf));

	return in_buf;
}

static u32 ext2_read_indirect(u32 indirect, u32 block_num, struct dev *dev)
{
	u8 buf[BLOCK_SIZE] = { 0 };
	ext2_buffer_read(indirect, buf, dev);
	u32 ind = *(u32 *)((u32)buf + block_num * sizeof(u32));
	return ind;
}

static s32 ext2_read_inode(struct ext2_inode *in, void *buf, u32 offset, u32 count, struct dev *dev)
{
	if (!in || !buf)
		return -1;

	if (in->size == 0)
		return 0;

	u32 num_blocks = in->blocks / (BLOCK_SIZE / SECTOR_SIZE) + 1;

	if (!num_blocks)
		return -1;

	u32 first_block = offset / BLOCK_SIZE;
	u32 last_block = (offset + count) / BLOCK_SIZE;
	if (last_block >= num_blocks)
		last_block = num_blocks - 1;
	u32 first_block_offset = offset % BLOCK_SIZE;

	u32 remaining = MIN(count, in->size - offset);
	u32 copied = 0;

	u32 indirect, blocknum;

	// TODO: Support triply indirect pointers
	for (u32 i = first_block; i <= last_block; i++) {
		if (i < 12) {
			blocknum = in->block[i];
		} else if (i < BLOCK_COUNT + 12) {
			indirect = in->block[12];
			blocknum = ext2_read_indirect(indirect, i - 12, dev);
		} else {
			indirect = in->block[13];
			blocknum = ext2_read_indirect(indirect,
						      (i - (BLOCK_COUNT + 12)) / BLOCK_COUNT, dev);
			blocknum = ext2_read_indirect(blocknum,
						      (i - (BLOCK_COUNT + 12)) % BLOCK_COUNT, dev);
		}

		u8 data[BLOCK_SIZE] = { 0 };
		ext2_buffer_read(blocknum, data, dev);
		u32 block_offset = (i == first_block) ? first_block_offset : 0;
		u32 byte_count = MIN(BLOCK_SIZE - block_offset, remaining);

		memcpy((u8 *)buf + copied, data + block_offset, byte_count);

		copied += byte_count;
		remaining -= byte_count;
	}

	return copied;
}

static u32 ext2_find_inode(const char *name, u32 dir_inode, struct dev *dev)
{
	if ((signed)dir_inode <= 0)
		return (unsigned)-1;

	struct ext2_inode i = { 0 };
	ext2_get_inode(dir_inode, &i, dev);

	char buf[BLOCK_SIZE] = { 0 };
	assert(BLOCK_SIZE * i.blocks / 2 <= sizeof(buf)); // Shouldn't fail

	u8 data[BLOCK_SIZE] = { 0 };
	for (u32 q = 0; q < i.blocks / 2; q++) {
		ext2_buffer_read(i.block[q], data, dev);
		memcpy((u32 *)((u32)buf + q * BLOCK_SIZE), data, BLOCK_SIZE);
	}

	struct ext2_dirent *d = (struct ext2_dirent *)buf;

	u32 sum = 0;
	do {
		// Calculate the 4byte aligned size of each entry
		sum += d->total_len;
		if (strlen(name) == d->name_len &&
		    strncmp((void *)d->name, name, d->name_len) == 0) {
			return d->inode_num;
		}
		d = (struct ext2_dirent *)((u32)d + d->total_len);

	} while (sum < (BLOCK_SIZE * i.blocks / 2));

	return (unsigned)-1;
}

static struct ext2_inode *ext2_find_inode_by_path(const char *path, struct ext2_inode *in_buf,
						  struct dev *dev)
{
	char path_arr[1024] = { 0 };
	strlcpy(path_arr, path, sizeof(path_arr));
	char *path_cp = path_arr;

	if (path_cp[0] != '/')
		return NULL;

	path_cp++;
	u32 current_inode = EXT2_ROOT;

	while (1) {
		u32 i;
		for (i = 0; path_cp[i] != '/' && path_cp[i] != '\0'; i++)
			;

		if (path_cp[i] == '\0')
			break;

		path_cp[i] = '\0';
		current_inode = ext2_find_inode(path_cp, current_inode, dev);
		path_cp[i] = '/';

		if ((signed)current_inode <= 0) {
			return NULL;
		}

		path_cp += i + 1;
	}

	u32 inode = ext2_find_inode(path_cp, current_inode, dev);
	if ((signed)inode <= 0)
		return NULL;

	return ext2_get_inode(inode, in_buf, dev);
}

static s32 ext2_read(const char *path, void *buf, u32 offset, u32 count, struct dev *dev)
{
	struct ext2_inode in = { 0 };
	if (ext2_find_inode_by_path(path, &in, dev) == &in) {
		return ext2_read_inode(&in, buf, offset, count, dev);
	} else {
		return -1;
	}
}

u8 ext2_detect(struct dev *dev)
{
	struct ext2_superblock sb = { 0 };
	ext2_get_superblock(&sb, dev);
	if (sb.magic != EXT2_MAGIC)
		return 0;

	dev->p.disk.fs.read = ext2_read;

	return 1;
}
