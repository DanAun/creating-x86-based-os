#include "gdt_setup.h"



void create_gdt_with_required_desc(seg_desc_t *gdtr_address, tss_t * TSS) {
  if (!gdtr_address)
    return;

  // Initialize null descriptor
  seg_desc_t *desc_0 = &gdtr_address[0];
  initialize_segment_descriptor(desc_0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  // Initialize code segment descriptor
  seg_desc_t *desc_1 = &gdtr_address[1];
  initialize_segment_descriptor(desc_1, 0, 0xFFFFF, 0b1011, 0b00, 1, 0, 1, 1,
                                1);

  // Initialize data segment descriptor
  seg_desc_t *desc_2 = &gdtr_address[2];
  initialize_segment_descriptor(desc_2, 0, 0xFFFF, 0b0011, 0b00, 1, 0, 1, 1, 1);

  // Initialize specific data segment descriptor
  seg_desc_t *desc_3 = &gdtr_address[3];
  initialize_segment_descriptor(desc_3, 0x600000, 0x20, 0b0011, 0b00, 0, 0, 1,
                                1, 1);

  //    * **Code, 32 bits RX, ring 3, flat**
  seg_desc_t *desc_4 = &gdtr_address[4];
  initialize_segment_descriptor(desc_4, 0x300000, 0xfffff, 0b1011, 0b11, 1, 0, 1, 1,
                                1);
  //    * **Data, 32 bits RW, ring 3, flat**
  seg_desc_t *desc_5 = &gdtr_address[5];
  initialize_segment_descriptor(desc_5, 0x0, 0xfffff, 0b0011, 0b11, 1, 0, 1, 1,
                                1);

  // Task State Segment descriptor
  seg_desc_t *desc_6 = &gdtr_address[6];
  initialize_segment_descriptor(desc_6, (offset_t)&TSS, sizeof(tss_t),
                                SEG_DESC_SYS_TSS_AVL_32, 0b00, 0, 0, 0, 0, 1);
}
