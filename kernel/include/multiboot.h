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
 *
 *  This header is based on code from GNU GRUB2.
 *  For more information, see http://www.gnu.org/software/grub/
 */
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_HEADER_MAGIC (0x1BADB002)

#define MULTIBOOT_HEADER_FEATURE_PAGE_ALIGN (0x00000001)

#define MULTIBOOT_HEADER_FEATURE_AOUT_KLUDGE (0x00010000)

typedef struct multiboot_header_s
{
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

typedef struct elf_section_header_table
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} __attribute__((packed)) elf_section_header_table_t;

typedef struct memory_map
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) memory_map_t;

typedef struct multiboot_module
{
    uint32_t start;
    uint32_t end;
    const char *cmdline;
    uint32_t pad;
} __attribute__((packed)) multiboot_module_t;

typedef struct multiboot_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    const char* cmdline;
    uint32_t mods_count;
    multiboot_module_t *mods_addr;
    elf_section_header_table_t elf_sec;
    uint32_t mmap_length;
    const memory_map_t *mmap_addr;
} __attribute__((packed)) multiboot_info_t;

#define MULTIBOOT_INFO_MEMORY			0x00000001
#define MULTIBOOT_INFO_BOOTDEV			0x00000002
#define MULTIBOOT_INFO_CMDLINE			0x00000004
#define MULTIBOOT_INFO_MODS			0x00000008
#define MULTIBOOT_INFO_MEM_MAP			0x00000040

extern multiboot_info_t *mbi;

#endif /* MULTIBOOT_H */
