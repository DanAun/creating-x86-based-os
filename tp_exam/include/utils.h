
#ifndef UTILS
#define UTILS

#include "gpr.h"
#include "types.h"
#include <debug.h>
#include <segmem.h>
#include <intr.h>

void print_selector_content(short selector);

void display_tss(const tss_t *tss) ;
// Function to initialize a segment descriptor
void initialize_segment_descriptor(seg_desc_t *desc, uint32_t base,
                                   uint32_t limit, uint8_t type, uint8_t dpl,
                                   uint8_t g, uint8_t l, uint8_t avl, uint8_t s,
                                   uint8_t p) ;


void display_type(uint64_t system_field, uint64_t type_field) ;

void print_gdt_content(gdt_reg_t gdtr_ptr) ;


void display_usefull_info() ;
#endif // !DEBUG
