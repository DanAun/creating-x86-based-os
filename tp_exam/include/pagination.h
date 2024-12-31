#ifdef PAGINATION_
#define PAGINATION_ 

#include "cr.h"
#include "pagemem.h"
#include "types.h"
#include <debug.h>

void map_addresses(pde32_t *pgd ,uint32_t v_start, uint32_t p_start, uint32_t size) ;

#endif // DEBUG
