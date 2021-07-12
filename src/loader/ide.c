// MIT License, Copyright (c) 2021 Marvin Borner

#include <cpu.h>
#include <def.h>
#include <device.h>
#include <ide.h>
#include <library.h>
#include <log.h>
#include <panic.h>

static void ide_select_drive(u8 bus, u8 drive)
{
	if (bus == ATA_PRIMARY) {
		if (drive == ATA_MASTER)
			outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xa0);
		else
			outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xb0);
	} else {
		if (drive == ATA_MASTER)
			outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xa0);
		else
			outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xb0);
	}
}

static u8 ide_find(u8 bus, u8 drive)
{
	static u8 ide_buf[SECTOR_SIZE] = { 0 };

	u16 io = bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
	ide_select_drive(bus, drive);

	// Reset
	outb(io + ATA_REG_SECCOUNT0, 0);
	outb(io + ATA_REG_LBA0, 0);
	outb(io + ATA_REG_LBA1, 0);
	outb(io + ATA_REG_LBA2, 0);

	// Identify
	outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	u8 status = inb(io + ATA_REG_STATUS);
	if (!status)
		return 0;

	while ((inb(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0)
		;

	do {
		status = inb(io + ATA_REG_STATUS);
		if (status & ATA_SR_ERR)
			return 0;
	} while ((status & ATA_SR_DRQ) == 0);

	for (u16 i = 0; i < BLOCK_COUNT; i++)
		*(u16 *)(ide_buf + i * 2) = inw(io + ATA_REG_DATA);

	return 1;
}

static void ide_delay(u16 io) // 400ns
{
	for (int i = 0; i < 4; i++)
		inb(io + ATA_REG_ALTSTATUS);
}

static void ide_poll(u16 io)
{
	while (inb(io + ATA_REG_STATUS) & ATA_SR_BSY)
		;

	assert(!(inb(io + ATA_REG_STATUS) & ATA_SR_ERR));
}

static s32 ata_read(void *buf, u32 lba, u32 sector_count, struct dev *dev)
{
	u8 drive = dev->data & 0xff;
	u16 io = (drive & ATA_PRIMARY << 1) == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
	drive = (drive & ATA_SLAVE) == ATA_SLAVE ? ATA_SLAVE : ATA_MASTER;
	u8 cmd = drive == ATA_MASTER ? 0xe0 : 0xf0;

	outb(io + ATA_REG_HDDEVSEL, (cmd | (u8)((lba >> 24 & 0x0f))));
	outb(io + ATA_REG_FEATURES, 0);
	outb(io + ATA_REG_SECCOUNT0, sector_count);
	outb(io + ATA_REG_LBA0, (u8)lba);
	outb(io + ATA_REG_LBA1, (u8)(lba >> 8));
	outb(io + ATA_REG_LBA2, (u8)(lba >> 16));
	outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	u16 *b = buf;
	u32 count = sector_count;
	while (count-- > 0) {
		ide_poll(io);
		__asm__ volatile("rep insw" ::"c"(BLOCK_COUNT), "d"(io + ATA_REG_DATA),
				 "D"((u32)b));
	}

	ide_delay(io);
	return sector_count;
}

static void ata_probe(void)
{
	for (u8 i = 0; i < 4; i++) {
		u32 bus = i < 2 ? ATA_PRIMARY : ATA_SECONDARY;
		u32 drive = i % 2 ? ATA_MASTER : ATA_SLAVE;
		u8 data = (bus << 1) | drive;

		if (!ide_find(bus, drive))
			continue;

		char name[4] = { 0 };
		strlcpy(name, "hd", sizeof(name));
		name[2] = 'a' + i;

		// Register without write support
		device_register(DEVICE_DISK, name, data, ata_read, NULL);
	}
}

void ata_install(void)
{
	ata_probe();
}
