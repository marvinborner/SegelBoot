// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef PCI_H
#define PCI_H

#include <def.h>

// From PCI spec
#define PCI_VENDOR_ID 0x00 // 2
#define PCI_DEVICE_ID 0x02 // 2
#define PCI_COMMAND 0x04 // 2
#define PCI_STATUS 0x06 // 2
#define PCI_REVISION_ID 0x08 // 1

#define PCI_PROG_IF 0x09 // 1
#define PCI_SUBCLASS 0x0a // 1
#define PCI_CLASS 0x0b // 1
#define PCI_CACHE_LINE_SIZE 0x0c // 1
#define PCI_LATENCY_TIMER 0x0d // 1
#define PCI_HEADER_TYPE 0x0e // 1
#define PCI_BIST 0x0f // 1
#define PCI_BAR0 0x10 // 4
#define PCI_BAR1 0x14 // 4
#define PCI_BAR2 0x18 // 4
#define PCI_BAR3 0x1C // 4
#define PCI_BAR4 0x20 // 4
#define PCI_BAR5 0x24 // 4

#define PCI_INTERRUPT_LINE 0x3C // 1

#define PCI_SECONDARY_BUS 0x19 // 1

#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_TYPE_BRIDGE 0x060400
#define PCI_TYPE_SATA 0x010600

#define PCI_ADDR_PORT 0xcf8
#define PCI_VALUE_PORT 0xcfc

#define PCI_NONE 0xffff

void pci_probe(void);

/**
 * Some common PCI IDs
 */

#define PCI_IDE(vendor_id, device_id)                                                              \
	(vendor_id == 0x8086 && (device_id == 0x7010 || device_id == 0x7111))

static const struct {
	u16 id;
	const char *name;
} pci_vendors[] = {
	{ 0x1022, "AMD" },	  { 0x106b, "Apple, Inc." }, { 0x1234, "Bochs/QEMU" },
	{ 0x1274, "Ensoniq" },	  { 0x15ad, "VMWare" },	     { 0x8086, "Intel Corporation" },
	{ 0x80EE, "VirtualBox" },
};

static const struct {
	u16 vendor_id;
	u16 device_id;
	const char *name;
} pci_devices[] = {
	{ 0x1022, 0x2000, "PCNet Ethernet Controller (pcnet)" },
	{ 0x106b, 0x003f, "OHCI Controller" },
	{ 0x1234, 0x1111, "VGA BIOS Graphics Extensions" },
	{ 0x1274, 0x1371, "Creative Labs CT2518 (ensoniq audio)" },
	{ 0x15ad, 0x0740, "VM Communication Interface" },
	{ 0x15ad, 0x0405, "SVGA II Adapter" },
	{ 0x15ad, 0x0790, "PCI bridge" },
	{ 0x15ad, 0x07a0, "PCI Express Root Port" },
	{ 0x8086, 0x100e, "Gigabit Ethernet Controller (e1000)" },
	{ 0x8086, 0x100f, "Gigabit Ethernet Controller (e1000)" },
	{ 0x8086, 0x1237, "PCI & Memory" },
	{ 0x8086, 0x2415, "AC'97 Audio Chipset" },
	{ 0x8086, 0x7000, "PCI-to-ISA Bridge" },
	{ 0x8086, 0x7010, "PIIX3 IDE" },
	{ 0x8086, 0x7110, "PIIX4 ISA" },
	{ 0x8086, 0x7111, "PIIX4 IDE" },
	{ 0x8086, 0x7113, "Power Management Controller" },
	{ 0x8086, 0x7190, "Host Bridge" },
	{ 0x8086, 0x7191, "AGP Bridge" },
	{ 0x80EE, 0xBEEF, "Bochs/QEMU-compatible Graphics Adapter" },
	{ 0x80EE, 0xCAFE, "Guest Additions Device" },
};

static const struct {
	u32 id : 24; // Only 24 Bit
	const char *name;
} pci_types[] = {
	{ 0x000000, "Legacy Device" },
	{ 0x000100, "VGA-Compatible Device" },

	{ 0x010000, "SCSI bus controller" },
	{ 0x010100, "ISA Compatibility mode-only controller" },
	{ 0x010105, "PCI native mode-only controller" },
	{ 0x01010a, "ISA Compatibility mode controller, supports both channels "
		    "switched to PCI native mode" },
	{ 0x01010f, "PCI native mode controller, supports both channels switched "
		    "to ISA compatibility mode" },
	{ 0x010180, "ISA Compatibility mode-only controller, supports bus mastering" },
	{ 0x010185, "PCI native mode-only controller, supports bus mastering" },
	{ 0x01018a, "ISA Compatibility mode controller, supports both channels "
		    "switched to PCI native mode, supports bus mastering" },
	{ 0x01018f, "PCI native mode controller, supports both channels switched "
		    "\to ISA compatibility mode, supports bus mastering" },

	{ 0x010200, "Floppy disk controller" },
	{ 0x010300, "IPI bus controller" },
	{ 0x010400, "RAID controller" },
	{ 0x010520, "ATA controller, single stepping" },
	{ 0x010530, "ATA controller, continuous" },
	{ 0x010600, "Serial ATA controller - vendor specific interface" },
	{ 0x010601, "Serial ATA controller - AHCI 1.0 interface" },
	{ 0x010700, "Serial Attached SCSI controller" },
	{ 0x018000, "Mass Storage controller" },

	{ 0x020000, "Ethernet controller" },
	{ 0x020100, "Token Ring controller" },
	{ 0x020200, "FDDI controller" },
	{ 0x020300, "ATM controller" },
	{ 0x020400, "ISDN controller" },
	{ 0x020500, "WorldFip controller" },
	// { 0x0206xx , "PICMG 2.14 Multi Computing" },
	{ 0x028000, "Network controller" },

	{ 0x030000, "VGA Display controller" },
	{ 0x030001, "8514-compatible Display controller" },
	{ 0x030100, "XGA Display controller" },
	{ 0x030200, "3D Display controller" },
	{ 0x038000, "Display controller" },

	{ 0x040000, "Video device" },
	{ 0x040100, "Audio device" },
	{ 0x040200, "Computer Telephony device" },
	{ 0x048000, "Multimedia device" },

	{ 0x050000, "RAM memory controller" },
	{ 0x050100, "Flash memory controller" },
	{ 0x058000, "Memory controller" },

	{ 0x060000, "Host bridge" },
	{ 0x060100, "ISA bridge" },
	{ 0x060200, "EISA bridge" },
	{ 0x060300, "MCA bridge" },
	{ 0x060400, "PCI-to-PCI bridge" },
	{ 0x060401, "PCI-to-PCI bridge (subtractive decoding)" },
	{ 0x060500, "PCMCIA bridge" },
	{ 0x060600, "NuBus bridge" },
	{ 0x060700, "CardBus bridge" },
	// { 0x0608xx , "RACEway bridge" },
	{ 0x060940, "PCI-to-PCI bridge, Semi-transparent, primary facing Host" },
	{ 0x060980, "PCI-to-PCI bridge, Semi-transparent, secondary facing Host" },
	{ 0x060A00, "InfiniBand-to-PCI host bridge" },
	{ 0x068000, "Bridge device" },

	{ 0x070000, "Generic XT-compatible serial controller" },
	{ 0x070001, "16450-compatible serial controller" },
	{ 0x070002, "16550-compatible serial controller" },
	{ 0x070003, "16650-compatible serial controller" },
	{ 0x070004, "16750-compatible serial controller" },
	{ 0x070005, "16850-compatible serial controller" },
	{ 0x070006, "16950-compatible serial controller" },

	{ 0x070100, "Parallel port" },
	{ 0x070101, "Bi-directional parallel port" },
	{ 0x070102, "ECP 1.X compliant parallel port" },
	{ 0x070103, "IEEE1284 controller" },
	{ 0x0701FE, "IEEE1284 target device" },
	{ 0x070200, "Multiport serial controller" },

	{ 0x070300, "Generic modem" },
	{ 0x070301, "Hayes 16450-compatible modem" },
	{ 0x070302, "Hayes 16550-compatible modem" },
	{ 0x070303, "Hayes 16650-compatible modem" },
	{ 0x070304, "Hayes 16750-compatible modem" },
	{ 0x070400, "GPIB (IEEE 488.1/2) controller" },
	{ 0x070500, "Smart Card" },
	{ 0x078000, "Communications device" },

	{ 0x080000, "Generic 8259 PIC" },
	{ 0x080001, "ISA PIC" },
	{ 0x080002, "EISA PIC" },
	{ 0x080010, "I/O APIC interrupt controller" },
	{ 0x080020, "I/O(x) APIC interrupt controller" },

	{ 0x080100, "Generic 8237 DMA controller" },
	{ 0x080101, "ISA DMA controller" },
	{ 0x080102, "EISA DMA controller" },

	{ 0x080200, "Generic 8254 system timer" },
	{ 0x080201, "ISA system timer" },
	{ 0x080202, "EISA system timer-pair" },

	{ 0x080300, "Generic RTC controller" },
	{ 0x080301, "ISA RTC controller" },

	{ 0x080400, "Generic PCI Hot-Plug controller" },
	{ 0x080500, "SD Host controller" },
	{ 0x088000, "System peripheral" },

	{ 0x090000, "Keyboard controller" },
	{ 0x090100, "Digitizer (pen)" },
	{ 0x090200, "Mouse controller" },
	{ 0x090300, "Scanner controller" },
	{ 0x090400, "Generic Gameport controller" },
	{ 0x090410, "Legacy Gameport controller" },
	{ 0x098000, "Input controller" },

	{ 0x0a0000, "Generic docking station" },
	{ 0x0a8000, "Docking station" },

	{ 0x0b0000, "386 Processor" },
	{ 0x0b0100, "486 Processor" },
	{ 0x0b0200, "Pentium Processor" },
	{ 0x0b1000, "Alpha Processor" },
	{ 0x0b2000, "PowerPC Processor" },
	{ 0x0b3000, "MIPS Processor" },
	{ 0x0b4000, "Co-processor" },

	{ 0x0c0000, "IEEE 1394 (FireWire)" },
	{ 0x0c0010, "IEEE 1394 -- OpenHCI spec" },
	{ 0x0c0100, "ACCESS.bus" },
	{ 0x0c0200, "SSA" },
	{ 0x0c0300, "Universal Serial Bus (UHC spec)" },
	{ 0x0c0310, "Universal Serial Bus (Open Host spec)" },
	{ 0x0c0320, "USB2 Host controller (Intel Enhanced HCI spec)" },
	{ 0x0c0380, "Universal Serial Bus (no PI spec)" },
	{ 0x0c03FE, "USB Target Device" },
	{ 0x0c0400, "Fibre Channel" },
	{ 0x0c0500, "System Management Bus" },
	{ 0x0c0600, "InfiniBand" },
	{ 0x0c0700, "IPMI SMIC Interface" },
	{ 0x0c0701, "IPMI Kybd Controller Style Interface" },
	{ 0x0c0702, "IPMI Block Transfer Interface" },
	// { 0x0c08xx , "SERCOS Interface" },
	{ 0x0c0900, "CANbus" },

	{ 0x0d100, "iRDA compatible controller" },
	{ 0x0d100, "Consumer IR controller" },
	{ 0x0d100, "RF controller" },
	{ 0x0d100, "Bluetooth controller" },
	{ 0x0d100, "Broadband controller" },
	{ 0x0d100, "Ethernet (802.11a 5 GHz) controller" },
	{ 0x0d100, "Ethernet (802.11b 2.4 GHz) controller" },
	{ 0x0d100, "Wireless controller" },

	// { 0x0e00xx , "I2O Intelligent I/O, spec 1.0" },
	{ 0x0e0000, "Message FIFO at offset 040h" },

	{ 0x0f0100, "TV satellite comm. controller" },
	{ 0x0f0200, "Audio satellite comm. controller" },
	{ 0x0f0300, "Voice satellite comm. controller" },
	{ 0x0f0400, "Data satellite comm. controller" },

	{ 0x100000, "Network and computing en/decryption" },
	{ 0x101000, "Entertainment en/decryption" },
	{ 0x108000, "En/Decryption" },

	{ 0x110000, "DPIO modules" },
	{ 0x110100, "Perf. counters" },
	{ 0x111000, "Comm. synch., time and freq. test" },
	{ 0x112000, "Management card" },
	{ 0x118000, "Data acq./Signal proc." },
};

#endif
