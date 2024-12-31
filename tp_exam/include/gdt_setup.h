#ifndef GDT_SETUP
#define GDT_SETUP

#include "gpr.h"
#include "types.h"
#include <debug.h>
#include <segmem.h>
#include <intr.h>
#include "utils.h"

void create_gdt_with_required_desc(seg_desc_t *gdtr_address, tss_t * TSS) ;

#endif // !GDT_SETUP
