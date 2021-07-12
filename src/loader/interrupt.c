// MIT License, Copyright (c) 2021 Marvin Borner

#include <interrupt.h>
#include <log.h>
#include <pic.h>
#include <panic.h>

/**
 * IDT
 */

extern u32 interrupt_table[];
static struct idt_entry idt_entries[256] = { 0 };
REAL static struct idt_ptr idt = { .size = sizeof(idt_entries) - 1, .base = idt_entries };

void idt_install(void)
{
	for (u8 i = 0; i < 3; i++)
		idt_entries[i] = IDT_ENTRY(interrupt_table[i], 0x18, INTERRUPT_GATE);

	idt_entries[3] = IDT_ENTRY(interrupt_table[3], 0x18, INTERRUPT_TRAP);
	idt_entries[4] = IDT_ENTRY(interrupt_table[4], 0x18, INTERRUPT_TRAP);

	for (u8 i = 5; i < 48; i++)
		idt_entries[i] = IDT_ENTRY(interrupt_table[i], 0x18, INTERRUPT_GATE);

	// Load table
	__asm__ volatile("lidt %0" : : "m"(idt) : "memory");
}

/**
 * Exception (trap) handling
 */

static const char *interrupt_trap_names[32] = {
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

static void interrupt_trap_handler(struct interrupt_frame *frame)
{
	static u8 faulting = 0;
	faulting++;

	if (faulting == 2) {
		// Fall back to serial driver
		serial_print("Double fault, halting immediatly\n");
		while (1)
			__asm__ volatile("cli\nhlt");
	}

	log("%s Exception (code %x) at 0x%x!\n", interrupt_trap_names[frame->interrupt_no], frame->err_code,
	    frame->eip);

	while (1)
		__asm__ volatile("cli\nhlt");
}

/**
 * Event handling
 */

static void (*interrupt_event_handlers[16])(void) = { 0 };

void interrupt_event_handler_add(u32 interrupt_no, void (*handler)(void))
{
	assert(interrupt_no < COUNT(interrupt_event_handlers));
	interrupt_event_handlers[interrupt_no] = handler;
}

static u32 interrupt_event_handler(struct interrupt_frame *frame)
{
	u32 interrupt_no = frame->interrupt_no - 32;
	assert(interrupt_no < COUNT(interrupt_event_handlers));
	void (*handler)(void) = interrupt_event_handlers[interrupt_no];
	if (handler)
		handler();

	return (u32)frame;
}

/**
 * Universal handler
 */

u32 interrupt_handler(u32 esp);
u32 interrupt_handler(u32 esp)
{
	struct interrupt_frame *frame = (struct interrupt_frame *)esp;
	if (frame->interrupt_no < 32)
		interrupt_trap_handler(frame);
	else if (frame->interrupt_no < 48)
		esp = interrupt_event_handler(frame);
	else
		panic("Unknown interrupt\n");

	pic_ack(frame->interrupt_no);
	return esp;
}
