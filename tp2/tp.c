/* GPLv2 (c) Airbus */
#include <debug.h>
#include <intr.h>

void bp_handler() {
	asm volatile ("pusha");
	debug("#BP handling\n");
	uint32_t eip;
	asm volatile ("mov 4(%%ebp), %0":"=r"(eip));
	debug("EIP = %x\n", (unsigned int) eip);
	asm volatile ("popa");
	asm volatile ("leave; iret");
}

void bp_trigger() {
    asm volatile ("int3");
	debug("Returned after BP\n");
}

void tp() {
	idt_reg_t idtr;
	get_idtr(idtr);
	debug("IDT addr:  0x%x ", (unsigned int) idtr.addr);
	int_desc_t *bp_dsc = &idtr.desc[3];
	bp_dsc->offset_1 = (uint16_t)((uint32_t)bp_handler);
	bp_dsc->offset_2 = (uint16_t)(((uint32_t)bp_handler)>>16);
	bp_trigger();
}
