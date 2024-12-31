#include "pagination.h"

void map_addresses(pde32_t *pgd ,uint32_t v_start, uint32_t p_start, uint32_t size, uint32_t flags) {
    debug("Mapping addresses: v_start=0x%x, p_start=0x%x, size=0x%x\n", v_start, p_start, size);

    // Step 1: Check if addresses and size are aligned
    if (v_start % PAGE_SIZE != 0 || p_start % PAGE_SIZE != 0 || size % PAGE_SIZE != 0) {
        debug("Error: Addresses or size are not aligned to page boundaries.\n");
        return;
    }

    // Step 2: Compute the number of pages to allocate
    uint32_t num_pages = size / PAGE_SIZE;
    debug("Number of pages to map: %u\n", num_pages);

    // Step 3: Determine if mapping spans multiple PGD entries
    uint32_t pgd_start_idx = v_start / (ENTRIES_PER_PTE * PAGE_SIZE);
    uint32_t pgd_end_idx = (v_start + size - 1) / (ENTRIES_PER_PTE * PAGE_SIZE);
    debug("PGD range: start_idx=%u, end_idx=%u\n", pgd_start_idx, pgd_end_idx);

    for (uint32_t pgd_idx = pgd_start_idx; pgd_idx <= pgd_end_idx; pgd_idx++) {
        debug("Processing PGD index: %u\n", pgd_idx);

        // Step 4: Reach for the right PGD entry
        if (pgd[pgd_idx].raw == 0) {
            uint32_t pte_base = PTB_ADDR_BASE + (pgd_idx * PTB_SIZE);
            debug("Allocating new PTE table at address: 0x%x\n", pte_base);
            pg_set_entry(&pgd[pgd_idx], flags, pte_base >> 12);
        } else {
            debug("PGD entry already allocated.\n");
        }

        pte32_t *pte = (pte32_t *)(pgd[pgd_idx].addr << 12);
        debug("Using PTE table at address: 0x%p\n", pte);

        // Step 5: Map pages within the current PTE table
        uint32_t pte_start_idx = (pgd_idx == pgd_start_idx) ? (v_start / PAGE_SIZE) % ENTRIES_PER_PTE : 0;
        uint32_t pte_end_idx = (pgd_idx == pgd_end_idx) ? ((v_start + size - 1) / PAGE_SIZE) % ENTRIES_PER_PTE : ENTRIES_PER_PTE - 1;
        debug("PTE range: start_idx=%u, end_idx=%u\n", pte_start_idx, pte_end_idx);

        for (uint32_t pte_idx = pte_start_idx; pte_idx <= pte_end_idx; pte_idx++) {
            debug("Mapping PTE index: %u, @ addr : 0x%08x, to physical address: 0x%x\n", pte_idx,(uint32_t)&pte[pte_idx], p_start);
            pg_set_entry(&pte[pte_idx], flags, p_start >> 12);
            p_start += PAGE_SIZE;
        }

        // Update virtual address for the next PGD entry
        v_start = (pgd_idx + 1) * ENTRIES_PER_PTE * PAGE_SIZE;
    }
    debug("Mapping complete.\n");
}
