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
#include <mem.h>
#include <intr.h>
#include <asm.h>
#include <ctors.h>
#include <multiboot.h>
#include <kprintf.h>
#include <array.h>

uint32_t __attribute__((section(".bss.page_aligned"), aligned(PAGE_SIZE))) kernel_page_directory[PAGE_TABLE_ENTRIES];
uint32_t __attribute__((section(".bss.page_aligned"), aligned(PAGE_SIZE))) kernel_page_tables[KERNEL_MEMORY_PAGES];

uint32_t memory_size;

extern char _initial_mapping_end;

uint32_t last_kernel_phaddr = PA((uint32_t)&_initial_mapping_end);

uint32_t last_kernel_vaddr;

// called before enabling paging
// all access to memory must be through PA and TPA macros
void memory_setup_paging(void)
{
    uint32_t pages = (*TPA(&last_kernel_phaddr) + (PAGE_SIZE-1)) >> PAGE_BITS;

    // build kernel page tables
    for(uint32_t value = 3, page_index = 0; page_index < pages; value += PAGE_SIZE, ++page_index)
        *TPA(kernel_page_tables + page_index) = value;

    // build initial kernel page directory
    for(uint32_t value = PA((uint32_t)kernel_page_tables) + 3, pd_index = 0; pd_index < KERNEL_MEMORY_PDES; value += PAGE_SIZE, ++pd_index) {
        *TPA(kernel_page_directory + pd_index) = value; // identity
        *TPA(kernel_page_directory + KERNEL_MEMORY_PDE_BASE + pd_index) = value; // high kernel
    }
}

// called after enabling paging
void memory_post_setup_paging(void)
{
    // remove the identity mapping
    for(uint32_t pd_index = 0; pd_index < KERNEL_MEMORY_PDES; ++pd_index) {
        kernel_page_directory[pd_index] = 0;
    }
}

// page fault handler
void pf_handler (void)
{
    freeze_system();
}

INTR_NO_ERROR(pf_isr, pf_handler);

static void memory_map_kernel_range(uint32_t phaddr, uint32_t phend)
{
    for(uint32_t value = phaddr + 3,
            page_index = phaddr >> PAGE_BITS,
            page_end = phend >> PAGE_BITS;
        page_index < page_end;
        ++page_index, value += PAGE_SIZE)
        kernel_page_tables[page_index] = value;
}

static void *bootheap_allocate(size_t size)
{
    uint32_t phaddr = PAGE_END(last_kernel_phaddr);
    void *result = (void*)VA(last_kernel_phaddr);
    last_kernel_phaddr += size;
    memory_map_kernel_range(phaddr, PAGE_END(last_kernel_phaddr));
    return result;
}

#define BUDDY_ORDER_LIMIT (16)

static page_t *pages;
static list_t pages_free_list[BUDDY_ORDER_LIMIT];

static int max_order_for_length(size_t length)
{
    int order = 0;
    for(; order < BUDDY_ORDER_LIMIT && (1u << order) <= length;
        ++order);
    return order-1;
}

static int max_order_for_index(size_t index)
{
    int order = 0;
    for(; order < BUDDY_ORDER_LIMIT-1 && !((1u << order) & index);
        ++order);
    return order;
}

static int max_order(size_t index, size_t max_length)
{
    int m1 = max_order_for_length(max_length);
    int m2 = max_order_for_index(index);
    return (m1 < m2) ? m1 : m2;
}

static void add_pages_to_pool(size_t from, size_t to)
{
    size_t index = from;
    while (index < to) {
        int order = max_order(index, to - index);
        page_t *page = &pages[index];
        list_insert_before(pages_free_list + order, &page->free_node);
        index += 1u << order;
    }
}

CONSTRUCTOR(mem)
{
    intr_set(0xe, pf_isr);
    const memory_map_t *mmap_end = (const memory_map_t*)(((const char*)mbi->mmap_addr) + mbi->mmap_length);
    if (!memory_size) {
        for(const memory_map_t *mmap_addr = mbi->mmap_addr;
            mmap_addr < mmap_end;
            mmap_addr = (const memory_map_t*)((char*)&mmap_addr->base_addr + mmap_addr->size)) {
            if (mmap_addr->type != 1)
                continue;
            uint64_t end = mmap_addr->base_addr + mmap_addr->length;
            if (end >> 32)
                continue;
            if (end > memory_size)
                memory_size = end;
        }
    }
    last_kernel_phaddr = (last_kernel_phaddr + 3) &~3;

    size_t pages_count = (memory_size+(PAGE_SIZE-1))>>PAGE_BITS;

    pages = bootheap_allocate(sizeof(*pages) * pages_count);

    last_kernel_phaddr = (last_kernel_phaddr + (PAGE_SIZE-1)) &~(PAGE_SIZE-1);

    ARRAY_FOREACH(node, pages_free_list)
        list_init(node);
    ARRAY_DYNAMIC_FOREACH(page, pages, pages_count) {
        list_init(&page->free_node);
        page->order = -1;
    }
    for(const memory_map_t *mmap_addr = mbi->mmap_addr;
        mmap_addr < mmap_end;
        mmap_addr = (const memory_map_t*)((char*)&mmap_addr->base_addr + mmap_addr->size)) {
        if (mmap_addr->type != 1)
            continue;
        uint64_t end = mmap_addr->base_addr + mmap_addr->length;
        if (end >> 32)
            continue;
        if (end <= last_kernel_phaddr)
            continue;
        uint32_t base = mmap_addr->base_addr;
        if (base >= memory_size)
            continue;
        if (base <= last_kernel_phaddr)
            base = last_kernel_phaddr;
        add_pages_to_pool(base >> PAGE_BITS, (end + (PAGE_SIZE-1)) >> PAGE_BITS);
    }
    last_kernel_vaddr = VA(last_kernel_phaddr);
}

page_t *page_allocate(unsigned order)
{
    unsigned o = order;
    for(; o < BUDDY_ORDER_LIMIT && list_is_empty(pages_free_list + o); ++o);
    if (o == BUDDY_ORDER_LIMIT)
        return NULL;
    while (o > order) {
        page_t *page = LIST_ELEMENT_AT(pages_free_list[o].next, page_t, free_node);
        size_t index = page - pages;
        list_remove(pages_free_list[o].next);
        --o;
        list_insert_before(pages_free_list + o, &page->free_node);
        list_insert_before(pages_free_list + o, &pages[index ^ (1 << o)].free_node);
    }
    {
        page_t *page = LIST_ELEMENT_AT(pages_free_list[order].next, page_t, free_node);
        list_remove(pages_free_list[order].next);
        page->order = order;
        return page;
    }
}

void page_free(page_t *page)
{
    if (-1 == page->order)
        return;
    size_t index = page - pages;
    int order = page->order;

    while (order < BUDDY_ORDER_LIMIT-1) {
        page_t *buddy = pages + (index^(1<<order));
        if (list_is_empty(&buddy->free_node))
            break;
        list_remove(&buddy->free_node);
        index &= ~(1 << order);
        ++order;
    }
    page = pages + index;
    list_insert_before(pages_free_list + order, &page->free_node);
    page->order = -1;
}

void *kernel_sbrk(intptr_t increment)
{
    void *result = (void*)last_kernel_vaddr;
    uint32_t next_sbrk = last_kernel_vaddr + increment;
    uint32_t last_allocated = (last_kernel_vaddr + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
    uint32_t next_allocated = (next_sbrk + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
    if (increment > 0) {
        while (last_allocated < next_allocated) {
            page_t *page = page_allocate(0);
            if (NULL == page) PANIC("no more memory");
            kernel_page_tables[PA(last_allocated) >> PAGE_BITS] = ((page - pages) << PAGE_BITS) | 3;
            last_allocated += PAGE_SIZE;
        }
    }
    else if (increment < 0) {
        while (last_allocated > next_allocated) {
            last_allocated -= PAGE_SIZE;
            uint32_t *page_entry = &kernel_page_tables[PA(last_allocated) >> PAGE_BITS];
            page_t *page = pages + (*page_entry >> PAGE_BITS);
            *page_entry = 0;
            page_free(page);
        }
    }
    last_kernel_vaddr = next_sbrk;
    return result;
}
