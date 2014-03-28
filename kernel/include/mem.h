/*
 *  borfos - a minimal OS for x86
 *  Copyright (C) 2013 Boris Figovsky.
 *
 *  borfos is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  borfos is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with borfos.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MEM_H
#define MEM_H

#define PAGE_BITS (12)
#define PAGE_SIZE (1 << PAGE_BITS)

#define PAGE_TABLE_ENTRIES_BITS (PAGE_BITS - 2)
#define PAGE_TABLE_ENTRIES (1 << PAGE_TABLE_ENTRIES_BITS)

#define PHYSICAL_BASE_ADDRESS  (0x100000)
#define KERNEL_MEMORY_BASE (0xc0000000)
#define VIRTUAL_BASE_ADDRESS (KERNEL_MEMORY_BASE + PHYSICAL_BASE_ADDRESS)

#define KERNEL_MEMORY_SIZE (uint32_t)(-KERNEL_MEMORY_BASE)
#define KERNEL_MEMORY_PAGES (KERNEL_MEMORY_SIZE >> PAGE_BITS)
#define KERNEL_MEMORY_PDES (KERNEL_MEMORY_PAGES >> PAGE_TABLE_ENTRIES_BITS)
#define KERNEL_MEMORY_PDE_BASE (KERNEL_MEMORY_BASE >> (PAGE_BITS+PAGE_TABLE_ENTRIES_BITS))

#define PA(vaddr) ((vaddr)-VIRTUAL_BASE_ADDRESS+PHYSICAL_BASE_ADDRESS)
#define VA(paddr) ((paddr)+VIRTUAL_BASE_ADDRESS-PHYSICAL_BASE_ADDRESS)

#ifndef ASM

#include <stdint.h>
#include <list.h>

#define TPA(ptr) ((typeof(ptr))PA((uint32_t)(ptr)))
#define TVA(ptr) ((typeof(ptr))VA((uint32_t)(ptr)))

#define PAGE_START(addr) ((addr)&~(PAGE_SIZE-1))
#define PAGE_END(addr) (((addr)+(PAGE_SIZE-1))&~(PAGE_SIZE-1))

extern uint32_t memory_size;
extern uint32_t last_kernel_phaddr;
//extern uint32_t kernel_page_directory[PAGE_TABLE_ENTRIES];

static inline void reserve_up_to_phaddr_before_paging(uint32_t phaddr)
{
    if (phaddr > *TPA(&last_kernel_phaddr))
        *TPA(&last_kernel_phaddr) = phaddr;
}

typedef struct page_s {
    list_t free_node; // if not empty, belongs to a free-list
    int order; // allocated order
    void *kernel_vaddr;
    unsigned kernel_vaddr_refcnt;
} page_t;

page_t *page_allocate(unsigned order);
void page_free(page_t *);

void *map_phaddr_in_kernel(uint32_t phaddr, unsigned order);
void unmap_phaddr_in_kernel(void *addr);

void *page_map(page_t *page);
void page_unmap(page_t *page);

void *kernel_page_allocate(unsigned order);
void kernel_page_free(void *addr);

#endif /* !ASM */

#endif /* MEM_H */
