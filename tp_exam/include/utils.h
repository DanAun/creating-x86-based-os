
#ifndef UTILS
#define UTILS

#include "cr.h"
#include "pagemem.h"
#include <debug.h>
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


void print_block_info(uint32_t start_vaddr, uint32_t end_vaddr, uint32_t phys_start, 
                     int is_large_page, uint32_t flags, int start_pde, int start_pte, int end_pte) ;

void analyze_page_mapping(pde32_t *ptb) ;

void display_cr3(const cr3_reg_t *cr3) ;

void display_cr0(const cr0_reg_t * cr0) ;
void display_pde(pde32_t* pde_ptr) ;

void display_pde_page(pde32_t* pde_ptr) ;

void display_pte(pte32_t* pte_ptr) ;


void display_usefull_info() ;
#endif // !DEBUG
