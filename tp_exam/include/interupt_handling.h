#ifndef INTERUPT_HANDLING
#define INTERUPT_HANDLING

#include <debug.h>
#include <intr.h>
#include <segmem.h>

typedef void (*handler_fn)();
void display_idt_register(const idt_reg_t *idt_reg) ;

void display_interrupt_descriptor(const int_desc_t *desc) ;

// Function to register an interrupt handler
void register_interrupt_handler(idt_reg_t* idtr, uint8_t interrupt_num, handler_fn handler) ;

#endif // !INTERUPUT_HANDLING
