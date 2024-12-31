#include "utils.h"

void print_selector_content(short selector) {
  debug("[ index : %d || TI : %s  || RPL : %d ]\n", selector >> 3,
        (selector >> 2 & 1) ? "LDT" : "GDT", selector & 3);
}

void display_tss(const tss_t *tss) {
  debug("=== Task State Segment (TSS) ===\n");

  debug("Back Link: 0x%04X\n", tss->back_link);
  debug("Stack 0: ESP=0x%08x, SS=0b%b\n", tss->s0.esp, tss->s0.ss);
    print_selector_content(tss->s0.ss);
  debug("Stack 1: ESP=0x%08x, SS=0b%b\n", tss->s1.esp, tss->s1.ss);
    print_selector_content(tss->s2.ss);
  debug("Stack 2: ESP=0x%08x, SS=0b%b\n", tss->s2.esp, tss->s2.ss);
    print_selector_content(tss->s2.ss);

  debug("CR3: 0x%08x\n", tss->cr3);
  debug("EIP: 0x%08x\n", tss->eip);

  debug("Segment Selectors:\n");
  debug("  ES: 0b%b\n", tss->es);
  debug("  CS: 0b%b\n", tss->cs);
  debug("  SS: 0b%b\n", tss->ss);
    print_selector_content(tss->ss);
  debug("  DS: 0b%b\n", tss->ds);
  debug("  FS: 0b%b\n", tss->fs);
  debug("  GS: 0b%b\n", tss->gs);
  debug("  LDT: 0b%b\n", tss->ldt);

  debug("Trap: %s\n", tss->trap.on ? "Enabled" : "Disabled");
}
// Function to initialize a segment descriptor
void initialize_segment_descriptor(seg_desc_t *desc, uint32_t base,
                                   uint32_t limit, uint8_t type, uint8_t dpl,
                                   uint8_t g, uint8_t l, uint8_t avl, uint8_t s,
                                   uint8_t p) {
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
  debug("tr : %b\n", tr);
  print_selector_content(tr);

  short cs = get_seg_sel(cs);
  debug("cs : %b\n", cs);
  print_selector_content(cs);

  short ds = get_seg_sel(ds);
  debug("ds : %b\n", ds);
  print_selector_content(ds);

  short ss = get_seg_sel(ss);
  debug("ss : %b\n", ss);
  print_selector_content(ss);

  short es = get_seg_sel(es);
  debug("es : %b\n", es);
  print_selector_content(es);

  short fs = get_seg_sel(fs);
  debug("fs : %b\n", fs);
  print_selector_content(fs);

  short gs = get_seg_sel(gs);
  debug("gs : %b\n", gs);
  print_selector_content(gs);


  // unsigned int new_gdt_ptr = 0xfffeffff;
  // offset_t new_gdt_ptr = gdtr.addr + gdtr.limit ;
  // gdt_reg_t new_gdtr = {new_gdt_ptr, {0x11}};
  // print_gdt_content(new_gdtr);
}
