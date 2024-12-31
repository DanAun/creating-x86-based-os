#include "utils.h"

void print_selector_content(short selector) {
  debug("[ index : %d || TI : %s  || RPL : %d ]\n", selector >> 3,
        (selector >> 2 & 1) ? "LDT" : "GDT", selector & 3);
}

void display_tss(const tss_t *tss) {
  debug("=== Task State Segment (TSS) ===\n");

  debug("Back Link: 0x%04X\n", tss->back_link);
  debug("Stack 0: ESP=0x%08x, SS=0x%x\n", tss->s0.esp, tss->s0.ss);
    print_selector_content(tss->s0.ss);
  debug("Stack 1: ESP=0x%08x, SS=0x%x\n", tss->s1.esp, tss->s1.ss);
    print_selector_content(tss->s2.ss);
  debug("Stack 2: ESP=0x%08x, SS=0x%x\n", tss->s2.esp, tss->s2.ss);
    print_selector_content(tss->s2.ss);

  debug("CR3: 0x%08x\n", tss->cr3);
  debug("EIP: 0x%08x\n", tss->eip);

  debug("Segment Selectors:\n");
  debug("  ES: 0x%x\n", tss->es);
  debug("  CS: 0x%x\n", tss->cs);
  debug("  SS: 0x%x\n", tss->ss);
    print_selector_content(tss->ss);
  debug("  DS: 0x%x\n", tss->ds);
  debug("  FS: 0x%x\n", tss->fs);
  debug("  GS: 0x%x\n", tss->gs);
  debug("  LDT: 0x%x\n", tss->ldt);

  debug("Trap: %s\n", tss->trap.on ? "Enabled" : "Disabled");
}
// Function to initialize a segment descriptor
void initialize_segment_descriptor(seg_desc_t *desc, uint32_t base,
                                   uint32_t limit, uint8_t type, uint8_t dpl,
                                   uint8_t g, uint8_t l, uint8_t avl, uint8_t s,
                                   uint8_t p) {
  debug("Initializing segment descriptor with base: 0x%08x, limit: 0x%08x, type: %d, dpl: %d, g: %d, l: %d, avl: %d, s: %d, p: %d\n", base, limit, type, dpl, g, l, avl, s, p);
  if (!desc)
    return;

  desc->base_1 = (base & 0xFFFF);
  desc->base_2 = (base >> 16) & 0xFF;
  desc->base_3 = (base >> 24) & 0xFF;

  desc->limit_1 = (limit & 0xFFFF);
  desc->limit_2 = (limit >> 16) & 0xF;

  desc->type = type;
  desc->dpl = dpl;
  desc->g = g;
  desc->l = l;
  desc->avl = avl;
  desc->s = s;
  desc->p = p;
}


void display_type(uint64_t system_field, uint64_t type_field) {
  if (!(system_field)) {
    // System segment type
    printf("| System Segment:");
    switch (type_field) {
    case 0x1:
      printf("16-bit TSS (Available) |");
      break;
    case 0x2:
      printf("LDT |");
      break;
    case 0x3:
      printf("16-bit TSS (Busy) |");
      break;
    case 0x4:
      printf("16-bit Call Gate |");
      break;
    case 0x5:
      printf("Task Gate |");
      break;
    case 0x6:
      printf("16-bit Interrupt Gate |");
      break;
    case 0x7:
      printf("16-bit Trap Gate |");
      break;
    case 0x9:
      printf("32-bit TSS (Available) |");
      break;
    case 0xB:
      printf("32-bit TSS (Busy) |");
      break;
    case 0xC:
      printf("32-bit Call Gate |");
      break;
    case 0xE:
      printf("32-bit Interrupt Gate |");
      break;
    case 0xF:
      printf("32-bit Trap Gate |");
      break;
    default:
      printf("Reserved or invalid system type |");
    }
  } else {
    // Code or Data segment
    printf("| %s Segment: ", (type_field & 0x08) ? "Code" : "Data");

    if (type_field & 0x08) {
      // Code segment specific flags
      printf("- Conforming: %s ", (type_field & 0x04) ? "Yes" : "No");
      printf("- Readable: %s ", (type_field & 0x02) ? "Yes" : "No");
      printf("- Accessed: %s ", (type_field & 0x01) ? "Yes" : "No");
    } else {
      // Data segment specific flags
      printf("- Expansion Direction: %s ", (type_field & 0x04) ? "Down" : "Up");
      printf("- Writable: %s ", (type_field & 0x02) ? "Yes" : "No");
      printf("- Accessed: %s ", (type_field & 0x01) ? "Yes" : "No");
    }
    printf(" |");
  }
}

void print_gdt_content(gdt_reg_t gdtr_ptr) {
  seg_desc_t *gdt_ptr;
  gdt_ptr = (seg_desc_t *)(gdtr_ptr.addr);
  int i = 0;
  debug("gtd addr : 0x%lx \n", gdtr_ptr.addr);
  debug("gtd size : %d \n", gdtr_ptr.limit);
  while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit)) {
    uint32_t start =
        gdt_ptr->base_3 << 24 | gdt_ptr->base_2 << 16 | gdt_ptr->base_1;
    uint32_t end;
    if (gdt_ptr->g) {
      end = start + ((gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1) << 12) + 4095;
    } else {
      end = start + (gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1);
    }
    debug("%d ", i);
    debug("[0x%x ", start);
    debug("- 0x%x] ", end);
    display_type(gdt_ptr->s, gdt_ptr->type);
    debug("desc_t: %d ", gdt_ptr->s);
    debug("priv: %d ", gdt_ptr->dpl);
    debug("present: %d ", gdt_ptr->p);
    debug("avl: %d ", gdt_ptr->avl);
    debug("longmode: %d ", gdt_ptr->l);
    debug("default: %d ", gdt_ptr->d);
    debug("gran: %d ", gdt_ptr->g);
    debug("\n");
    gdt_ptr++;
    i++;
  }
}


void display_usefull_info() {

  // TODO
  gdt_reg_t gdtr;
  get_gdtr(gdtr);
  print_gdt_content(gdtr);

  seg_desc_t *gdt_ptr;
  gdt_ptr = (seg_desc_t *)(gdtr.addr);
  debug("current gdt location : %p\n", gdt_ptr);

  debug("======= Segment Selectors =======\n");
  short tr;
  get_tr(tr);
  debug("tr : 0x%x\n", tr);
  print_selector_content(tr);

  short cs = get_seg_sel(cs);
  debug("cs : 0x%x\n", cs);
  print_selector_content(cs);

  short ds = get_seg_sel(ds);
  debug("ds : 0x%x\n", ds);
  print_selector_content(ds);

  short ss = get_seg_sel(ss);
  debug("ss : 0x%x\n", ss);
  print_selector_content(ss);

  short es = get_seg_sel(es);
  debug("es : 0x%x\n", es);
  print_selector_content(es);

  short fs = get_seg_sel(fs);
  debug("fs : 0x%x\n", fs);
  print_selector_content(fs);

  short gs = get_seg_sel(gs);
  debug("gs : 0x%x\n", gs);
  print_selector_content(gs);
}


void print_block_info(uint32_t start_vaddr, uint32_t end_vaddr, uint32_t phys_start, 
                     int is_large_page, uint32_t flags, int start_pde, int start_pte, int end_pte) {
    debug("Memory Block:\n");
    debug("  Virtual Range: 0x%08x - 0x%08x (Size: %dKB)\n", 
           start_vaddr, end_vaddr, (end_vaddr - start_vaddr + 1) / 1024);
    debug("  Physical Range: 0x%08x - 0x%08x\n",
           phys_start, phys_start + (end_vaddr - start_vaddr));
    debug("  Type: %s\n", is_large_page ? "4MB Page" : "4KB Pages");
    if (!is_large_page) {
        debug("  PDE: %d, PTE Range: %d-%d\n", start_pde, start_pte, end_pte);
    } else {
        debug("  PDE: %d\n", start_pde);
    }
    debug("  Flags: %s%s%s%s%s%s%s\n",
           (flags & 2) ? "RW " : "RO ",
           (flags & 4) ? "USER " : "SUPERVISOR ",
           (flags & 8) ? "PWT " : "",
           (flags & 16) ? "PCD " : "",
           (flags & 32) ? "ACC " : "",
           (flags & 64) ? "DIRTY " : "",
           (flags & 256) ? "GLOBAL " : "");
}

void analyze_page_mapping(pde32_t *ptb) {
    if (!ptb) {
        debug("Invalid page table pointer\n");
        return;
    }

    for (int pde_idx = 0; pde_idx < 1024; pde_idx++) {
        pde32_t *pde = &((pde32_t*)ptb)[pde_idx];
        
        if (!pde->p) continue;

        if (pde->page.ps) {
            print_block_info(
                pde_idx << 22,
                (pde_idx << 22) + 0x3FFFFF,
                pde->page.addr << 22,
                1,
                *(uint32_t*)pde & 0x3FF,
                pde_idx,
                0,
                0
            );
            continue;
        }

        pte32_t *pt = (pte32_t*)(pde->addr << 12);
        int block_start_pte = -1;
        uint32_t last_phys_addr = 0;
        uint32_t last_flags = 0;
        
        for (int pte_idx = 0; pte_idx < 1024; pte_idx++) {
            pte32_t *pte = &pt[pte_idx];
            uint32_t curr_flags = *(uint32_t*)pte & 0x3FF;
            uint32_t curr_phys = pte->addr << 12;
            
            if (!pte->p) {
                if (block_start_pte != -1) {
                    print_block_info(
                        (pde_idx << 22) | (block_start_pte << 12),
                        (pde_idx << 22) | ((pte_idx - 1) << 12) | 0xFFF,
                        last_phys_addr - ((pte_idx - 1 - block_start_pte) << 12),
                        0,
                        last_flags,
                        pde_idx,
                        block_start_pte,
                        pte_idx - 1
                    );
                    block_start_pte = -1;
                }
                continue;
            }

            if (block_start_pte == -1 || 
                curr_flags != last_flags || 
                curr_phys != last_phys_addr + 0x1000) {
                if (block_start_pte != -1) {
                    print_block_info(
                        (pde_idx << 22) | (block_start_pte << 12),
                        (pde_idx << 22) | ((pte_idx - 1) << 12) | 0xFFF,
                        last_phys_addr - ((pte_idx - 1 - block_start_pte) << 12),
                        0,
                        last_flags,
                        pde_idx,
                        block_start_pte,
                        pte_idx - 1
                    );
                }
                block_start_pte = pte_idx;
            }
            
            last_phys_addr = curr_phys;
            last_flags = curr_flags;
        }

        if (block_start_pte != -1) {
            print_block_info(
                (pde_idx << 22) | (block_start_pte << 12),
                (pde_idx << 22) | (1023 << 12) | 0xFFF,
                last_phys_addr - ((1023 - block_start_pte) << 12),
                0,
                last_flags,
                pde_idx,
                block_start_pte,
                1023
            );
        }
    }
}

void display_cr3(const cr3_reg_t *cr3) {
  printf("=== Control Register 3 (CR3) ===\n");
  printf("Reserved (r1): 0x%08x\n", cr3->r1);
  printf("Page Write Through (PWT): %u\n", cr3->pwt);
  printf("Page Cache Disable (PCD): %u\n", cr3->pcd);
  printf("Reserved (r2): 0x%08x\n", cr3->r2);
  printf("Physical Address (ADDR): 0x%86x\n", cr3->addr);
  printf("================================\n");
}

void display_cr0(const cr0_reg_t * cr0) {
  printf("Control Register 0 (CR0):\n");
  printf("  Protected Mode (PE): %u\n", cr0->pe);
  printf("  Monitor Coprocessor (MP): %u\n", cr0->mp);
  printf("  Emulation (EM): %u\n", cr0->em);
  printf("  Task Switch (TS): %u\n", cr0->ts);
  printf("  Extension Type (ET): %u\n", cr0->et);
  printf("  Numeric Error (NE): %u\n", cr0->ne);
  printf("  Reserved (R1): %u\n",
         cr0->r1); // Reserved fields can be shown as 0 or omitted
  printf("  Write Protect (WP): %u\n", cr0->wp);
  printf("  Reserved (R2): %u\n", cr0->r2);
  printf("  Align Mask (AM): %u\n", cr0->am);
  printf("  Reserved (R3): %u\n", cr0->r3);
  printf("  Not Write Through (NW): %u\n", cr0->nw);
  printf("  Cache Disable (CD): %u\n", cr0->cd);
  printf("  Paging (PG): %u\n", cr0->pg);
}
void display_pde(pde32_t* pde_ptr) {
    debug("Page Directory Entry:\n");
    debug("  Present (P): %u\n", pde_ptr->p);
    debug("  Read/Write (RW): %u\n", pde_ptr->rw);
    debug("  User/Supervisor (LVL): %u\n", pde_ptr->lvl);
    debug("  Page Write Through (PWT): %u\n", pde_ptr->pwt);
    debug("  Page Cache Disable (PCD): %u\n", pde_ptr->pcd);
    debug("  Accessed (ACC): %u\n", pde_ptr->acc);
    debug("  Must Be Zero (MBZ): %u\n", pde_ptr->mbz);
    debug("  Available (AVL): %u\n", pde_ptr->avl);
    debug("  Address: 0x%08x\n", pde_ptr->addr << 12);
}

void display_pde_page(pde32_t* pde_ptr) {
    debug("Page Directory Entry (Page):\n");
    debug("  Present (P): %u\n", pde_ptr->page.p);
    debug("  Read/Write (RW): %u\n", pde_ptr->page.rw);
    debug("  User/Supervisor (LVL): %u\n", pde_ptr->page.lvl);
    debug("  Page Write Through (PWT): %u\n", pde_ptr->page.pwt);
    debug("  Page Cache Disable (PCD): %u\n", pde_ptr->page.pcd);
    debug("  Accessed (ACC): %u\n", pde_ptr->page.acc);
    debug("  Dirty (D): %u\n", pde_ptr->page.d);
    debug("  Page Size (PS): %u\n", pde_ptr->page.ps);
    debug("  Global (G): %u\n", pde_ptr->page.g);
    debug("  Available (AVL): %u\n", pde_ptr->page.avl);
    debug("  Page Attribute Table (PAT): %u\n", pde_ptr->page.pat);
    debug("  Reserved (RSV): %u\n", pde_ptr->page.rsv);
    debug("  Address: 0x%03X\n", pde_ptr->page.addr);
}

void display_pte(pte32_t* pte_ptr) {
    debug("Page Table Entry:\n");
    debug("  Present (P): %u\n", pte_ptr->p);
    debug("  Read/Write (RW): %u\n", pte_ptr->rw);
    debug("  User/Supervisor (LVL): %u\n", pte_ptr->lvl);
    debug("  Page Write Through (PWT): %u\n", pte_ptr->pwt);
    debug("  Page Cache Disable (PCD): %u\n", pte_ptr->pcd);
    debug("  Accessed (ACC): %u\n", pte_ptr->acc);
    debug("  Dirty (D): %u\n", pte_ptr->d);
    debug("  Page Attribute Table (PAT): %u\n", pte_ptr->pat);
    debug("  Global (G): %u\n", pte_ptr->g);
    debug("  Available (AVL): %u\n", pte_ptr->avl);
    debug("  Address: 0x%08x\n", pte_ptr->addr<< 12);
}


