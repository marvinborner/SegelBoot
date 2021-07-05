// MIT License, Copyright (c) 2021 Marvin Borner

#include <int.h>
#include <log.h>
#include <pic.h>
#include <pnc.h>

/**
 * IDT
 */

extern u32 int_table[];
static struct idt_entry idt[256] = { 0 };
static struct idt_ptr idt_ptr = { 0 };

void idt_install(void)
{
	idt_ptr.size = sizeof(idt) - 1;
	idt_ptr.base = &idt;

	for (u8 i = 0; i < 3; i++)
		idt[i] = IDT_ENTRY(int_table[i], 0x08, INT_GATE);

	idt[3] = IDT_ENTRY(int_table[3], 0x08, INT_TRAP);
	idt[4] = IDT_ENTRY(int_table[4], 0x08, INT_TRAP);

	for (u8 i = 5; i < 48; i++)
		idt[i] = IDT_ENTRY(int_table[i], 0x08, INT_GATE);

	idt[128] = IDT_ENTRY(int_table[48], 0x08, INT_GATE | INT_USER);
	idt[129] = IDT_ENTRY(int_table[49], 0x08, INT_GATE);

	// Load table
	__asm__ volatile("lidt %0" : : "m"(idt_ptr));

	// Enable interrupts
	__asm__ volatile("sti");
}

/**
 * Exception (trap) handling
 */

const char *int_trap_names[32] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static void int_trap_handler(struct int_frame *frame)
{
	static u8 faulting = 0;
	faulting++;

	if (faulting == 2) {
		// Fall back to serial driver
		serial_print("Double fault, halting immediatly\n");
		while (1)
			__asm__ volatile("cli\nhlt");
	}

	log("%s Exception (code %x) at 0x%x!\n", int_trap_names[frame->int_no], frame->err_code,
	    frame->eip);

	while (1)
		__asm__ volatile("cli\nhlt");
}

/**
 * Event handling
 */

static void (*int_event_handlers[16])(void) = { 0 };

void int_event_handler_add(u32 int_no, void (*handler)(void))
{
	assert(int_no < COUNT(int_event_handlers));
	int_event_handlers[int_no] = handler;
}

static u32 int_event_handler(struct int_frame *frame)
{
	u32 int_no = frame->int_no - 32;
	assert(int_no < COUNT(int_event_handlers));
	void (*handler)(void) = int_event_handlers[int_no];
	if (handler)
		handler();

	return (u32)frame;
}

/**
 * Universal handler
 */

static u8 int_enabled = 1;

u32 int_handler(u32 esp);
u32 int_handler(u32 esp)
{
	struct int_frame *frame = (struct int_frame *)esp;
	if (frame->int_no < 32) {
		int_trap_handler(frame);
	} else if (int_enabled && frame->int_no < 48) {
		esp = int_event_handler(frame);
	} else if (frame->int_no >= 48) {
		panic("Unknown interrupt\n");
	}

	pic_ack(frame->int_no);
	return esp;
}
