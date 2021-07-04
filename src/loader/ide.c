// MIT License, Copyright (c) 2021 Marvin Borner

#include <cpu.h>
#include <def.h>
#include <ide.h>
#include <log.h>

static u8 ide_buf[SECTOR_SIZE] = { 0 };

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

	for (int i = 0; i < BLOCK_COUNT; i++)
		*(u16 *)(ide_buf + i * 2) = inw(io + ATA_REG_DATA);

	return 1;
}

static void ata_probe(void)
{
	for (u8 i = 0; i < 4; i++) {
		u32 bus = i < 2 ? ATA_PRIMARY : ATA_SECONDARY;
		u32 drive = i % 2 ? ATA_MASTER : ATA_SLAVE;

		if (!ide_find(bus, drive))
			continue;

		log("[IDE] Found hd%c\n", 'a' + i);
	}
}

void ata_install(void)
{
	ata_probe();
}
