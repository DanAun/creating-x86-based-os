#include <debug.h>
#include <segmem.h>
#include <pagemem.h>
#include <cr.h>

#include "utils.h"

seg_desc_t GDT[6];
tss_t      TSS;


#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idx  5

#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)
#define ts_sel  gdt_krn_seg_sel(ts_idx)

#define gdt_flat_dsc(_dSc_,_pVl_,_tYp_)                                 \
   ({                                                                   \
      (_dSc_)->raw     = 0;                                             \
      (_dSc_)->limit_1 = 0xffff;                                        \
      (_dSc_)->limit_2 = 0xf;                                           \
      (_dSc_)->type    = _tYp_;                                         \
      (_dSc_)->dpl     = _pVl_;                                         \
      (_dSc_)->d       = 1;                                             \
      (_dSc_)->g       = 1;                                             \
      (_dSc_)->s       = 1;                                             \
      (_dSc_)->p       = 1;                                             \
   })

#define tss_dsc(_dSc_,_tSs_)                                            \
   ({                                                                   \
      raw32_t addr    = {.raw = _tSs_};                                 \
      (_dSc_)->raw    = sizeof(tss_t);                                  \
      (_dSc_)->base_1 = addr.wlow;                                      \
      (_dSc_)->base_2 = addr._whigh.blow;                               \
      (_dSc_)->base_3 = addr._whigh.bhigh;                              \
      (_dSc_)->type   = SEG_DESC_SYS_TSS_AVL_32;                        \
      (_dSc_)->p      = 1;                                              \
   })

#define c0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_DATA_RW)

#define PDT_ADDR_KERNEL 0x100000
#define PDT_ADDR_PROCESS1 0x101000
#define PDT_ADDR_PROCESS2 0x102000

#define PT_ADDR_KERNEL 0x103000
#define PT_ADDR_PROCESS1 0x104000
#define PT_ADDR_PROCESS2 0x105000


void init_gdt() {
   gdt_reg_t gdtr;

   GDT[0].raw = 0ULL;

   c0_dsc( &GDT[c0_idx] );
   d0_dsc( &GDT[d0_idx] );
   c3_dsc( &GDT[c3_idx] );
   d3_dsc( &GDT[d3_idx] );

   gdtr.desc  = GDT;
   gdtr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr);

   set_cs(c0_sel);

   set_ss(d0_sel);
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);
}

void process1(){
	while(1){};
}

void process2(){
	while(1){};
}

void tp() {
	//init_gdt();

	//Set up TSS

 //  debug("=======Setting up TSS=======\n");
 //  debug("TSS location : %p\n", &TSS);
 //  TSS.s0.ss = make_seg_sel(2, 0, 0);
 //  TSS.s0.esp = get_esp();
 //  short tr = make_seg_sel(6, 0, 0);
 //  set_tr(tr);
 //  TSS.ss = make_seg_sel(5, 0, 3); // Ring 3 stack segment
 //   //
	// //Set up IDT
 //  debug("=======Setting up IDT=======\n");

	//Set up paging

	// Page Directory Tables (PDTs) for each process
	pde32_t *pdt_kernel = (pde32_t*) PDT_ADDR_KERNEL;
	set_cr3((uint32_t)pdt_kernel);
	pde32_t *pdt_process1 = (pde32_t*) PDT_ADDR_PROCESS1;
	pde32_t *pdt_process2 = (pde32_t*) PDT_ADDR_PROCESS2;
	
	// Page Tables (PTs) for each process
	pte32_t *pt_kernel = (pte32_t*) PT_ADDR_KERNEL;
	pte32_t *pt_process1 = (pte32_t*) PT_ADDR_PROCESS1;
	pte32_t *pt_process2 = (pte32_t*) PT_ADDR_PROCESS2;

	// Set up page tables
	for (uint32_t i = 0; i < PTE32_PER_PT; i++) {
		pg_set_entry(&pt_kernel[i], PG_KRN | PG_RW, i);
		pg_set_entry(&pt_process1[i], PG_USR | PG_RW, i);
		pg_set_entry(&pt_process2[i], PG_USR | PG_RW, i);
	}

	// Set up page directory tables
	memset((void*)pdt_kernel, 0, PAGE_SIZE);
	memset((void*)pdt_process1, 0, PAGE_SIZE);
	memset((void*)pdt_process2, 0, PAGE_SIZE);
	pg_set_entry(&pdt_kernel[0], PG_KRN|PG_RW, page_get_nr(pt_kernel));
	pg_set_entry(&pdt_process1[0], PG_USR|PG_RW, page_get_nr(pt_process1));
	pg_set_entry(&pdt_process2[0], PG_USR|PG_RW, page_get_nr(pt_process2));

	//Enable paging
	uint32_t cr0 = get_cr0();
	set_cr0(cr0|CR0_PG);

	analyze_page_mapping(&pdt_kernel[0]);
	analyze_page_mapping(&pdt_process1[0]);
	analyze_page_mapping(&pdt_process2[0]);
   
/* I need sleep here a super pagination function with example usages, have fun :
      *   map_addresses(pgd, 0x0, 0x0,
                0x1000 *
                    4); // Map 4 pages starting at virtual 0x0 to physical 0x0
  map_addresses(pgd,
      0x300000, 0x300000,
      0x1000 *
          7); // Map 7 pages starting at virtual 0x300000 to physical 0x300000
  map_addresses(pgd,
      0x600000, 0x600000,
      0x1000 *
          4); // Map 4 pages starting at virtual 0x600000 to physical 0x600000
    //
  map_addresses(pgd,
      0xc0000000, 0x600000,
      0x1000); // Map 4 pages starting at virtual 0x600000 to physical 0x600000
    //
*/



	debug("pt_kernel[1] = %x\n", pt_kernel[1].raw);
	debug("pt_process1[1] = %x\n", pt_process1[1].raw);
	debug("pt_process2[1] = %x\n", pt_process2[1].raw);
}
