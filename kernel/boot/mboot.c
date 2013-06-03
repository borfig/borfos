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
#include <multiboot.h>
#include <mem.h>
#include <cmdline.h>
#include <string.h>

extern uint8_t _load_start;
extern uint8_t _load_end;
extern uint8_t _bss_end;

void mboot_entry(void);

#define MULTIBOOT_HEADER_FLAGS                  \
    (MULTIBOOT_HEADER_FEATURE_PAGE_ALIGN|       \
     MULTIBOOT_HEADER_FEATURE_AOUT_KLUDGE)

multiboot_header_t multiboot __attribute__((section(".multiboot")))  = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_HEADER_FLAGS),
    PA((uint32_t)&multiboot),
    PA((uint32_t)&_load_start),
    PA((uint32_t)&_load_end),
    PA((uint32_t)&_bss_end),
    PA((uint32_t)&mboot_entry)
};

multiboot_info_t *mbi;

void initial_process_multiboot(multiboot_info_t *_mbi)
{
    *TPA(&mbi) = TVA(_mbi);

    if (_mbi->flags & MULTIBOOT_INFO_CMDLINE) {
        reserve_up_to_phaddr_before_paging((uint32_t)(_mbi->cmdline) + strlen(_mbi->cmdline) + 1);
        _mbi->cmdline = TVA(_mbi->cmdline);
    }
    if (_mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
        reserve_up_to_phaddr_before_paging((uint32_t)(_mbi->mmap_addr) + _mbi->mmap_length);
        _mbi->mmap_addr = TVA(_mbi->mmap_addr);
    }
    if (_mbi->flags & MULTIBOOT_INFO_MEMORY)
        *TPA(&memory_size) = (_mbi->mem_upper + 1024) << 10;

    if (_mbi->flags & MULTIBOOT_INFO_MODS) {
        reserve_up_to_phaddr_before_paging((uint32_t)(_mbi->mods_addr + _mbi->mods_count));
        for(multiboot_module_t *c = _mbi->mods_addr; c != _mbi->mods_addr + _mbi->mods_count; ++c) {
            reserve_up_to_phaddr_before_paging(c->end);
            reserve_up_to_phaddr_before_paging((uint32_t)(c->cmdline + strlen(c->cmdline)+1));
        }
        _mbi->mods_addr = TVA(_mbi->mods_addr);
    }
}
