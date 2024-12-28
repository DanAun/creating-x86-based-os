/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

char* intTypeToChar(unsigned int num){
   switch(num){
      case 1: return "MULTIBOOT_MEMORY_AVAILABLE";
      case 2: return "MULTIBOOT_MEMORY_RESERVED";
      case 3: return "MULTIBOOT_MEMORY_ACPI_RECLAIMABLE";
      case 4: return "MULTIBOOT_MEMORY_NVS";
      default: return "ERROR";
   }
}

void tp() {
   debug("kernel mem [0x%p - 0x%p]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);

   multiboot_memory_map_t* entry = (multiboot_memory_map_t*)info->mbi->mmap_addr;
   while((uint32_t)entry < (info->mbi->mmap_addr + info->mbi->mmap_length)) {
      debug("[0x%x - ", (unsigned int) entry->addr);
      debug("0x%x]", (unsigned int) (entry->len + entry->addr -1));
      debug(" %s\n", intTypeToChar(entry->type));

      entry++;
   }

   int *ptr_in_available_mem;
   ptr_in_available_mem = (int*)0x0;
   debug("Available mem (0x0): before: 0x%x ", *ptr_in_available_mem); // read
   *ptr_in_available_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_in_available_mem);                // check

   int *ptr_in_reserved_mem;
   ptr_in_reserved_mem = (int*)0xf0000;
   debug("Reserved mem (at: 0xf0000):  before: 0x%x ", *ptr_in_reserved_mem); // read
   *ptr_in_reserved_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_in_reserved_mem);                // check

   int *ptr_in_overflow_mem;
   ptr_in_overflow_mem = (int*)0x100000000;
   debug("Overflow mem (at: 0xffffffff):  before: 0x%x ", *ptr_in_overflow_mem); // read
   *ptr_in_overflow_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_in_overflow_mem);  

}
