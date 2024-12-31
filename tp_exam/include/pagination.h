#ifndef PAGINATION_
#define PAGINATION_ 

#include "cr.h"
#include "pagemem.h"
#include "types.h"
#include <debug.h>

#define PGD_ADDR 0x600000
#define PTB_ADDR_BASE 0x601000
#define PTB_COUNT 3
#define PTB_SIZE 0x1000

#define ENTRIES_PER_PTE 0x400
#define PG_SIZE 0x1000

void map_addresses(pde32_t *pgd ,uint32_t v_start, uint32_t p_start, uint32_t size) ;

#endif // DEBUG
