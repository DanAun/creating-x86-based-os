#include "interupt_handling.h"

void display_idt_register(const idt_reg_t *idt_reg) {
  if (!idt_reg) {
    debug("Error: NULL IDT register pointer\n");
    return;
  }

  debug("=== IDT Register Information ===\n");
  debug("Limit:        0x%04x (%u bytes)\n", idt_reg->limit,
        idt_reg->limit + 1);
  debug("Base Address: 0x%016lx\n", (unsigned long)idt_reg->addr);
  debug("\n");
}

void display_interrupt_descriptor(const int_desc_t *desc) {
  if (!desc) {
    debug("Error: NULL interrupt descriptor pointer\n");
    return;
  }

  debug("=== Interrupt Descriptor Information ===\n");
  debug("Offset:    0x%04lx%04lx (Full offset: 0x%08lx)\n",
        (unsigned long)desc->offset_2, (unsigned long)desc->offset_1,
        (unsigned long)((desc->offset_2 << 16) | desc->offset_1));
  debug("Selector:  0x%04lx\n", (unsigned long)desc->selector);
  debug("IST:       %lu\n", (unsigned long)desc->ist);
  debug("Type:      0x%lx ", (unsigned long)desc->type);

  // Decode gate type
  switch (desc->type) {
  case 0xE:
    debug("(64-bit Interrupt Gate)\n");
    break;
  case 0xF:
    debug("(64-bit Trap Gate)\n");
    break;
  default:
    debug("(Unknown Gate Type)\n");
  }

  debug("DPL:       %lu\n", (unsigned long)desc->dpl);
  debug("Present:   %s\n", desc->p ? "Yes" : "No");
  debug("\n");
}



// Function to register an interrupt handler
void register_interrupt_handler(idt_reg_t* idtr, uint8_t interrupt_num, handler_fn handler) {
    if (interrupt_num >= idtr->limit) {
        return;
    }
    
    int_desc_t* descriptor = &idtr->desc[interrupt_num];
    offset_t handler_addr = (offset_t)handler;
    
    descriptor->offset_1 = handler_addr & 0xFFFF;
    descriptor->offset_2 = (handler_addr >> 16) & 0xFFFF;
}

