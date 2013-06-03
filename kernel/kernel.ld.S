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
ENTRY(multiboot)
SECTIONS
{
    . = VIRTUAL_BASE_ADDRESS;
    _load_start = .;
    .multiboot : {
         *(.multiboot);
    }
    .text : {
         *(.text*);
    }
    .rodata : {
         *(.rodata*);
    }
    .data : {
         *(.data*);
    }
    _load_end = .;
    . = ALIGN(PAGE_SIZE);
    .bss : {
         *(.bss.page_aligned*);
         *(COMMON);
         *(.bss);
    }
    . = ALIGN(PAGE_SIZE) + PAGE_SIZE;
    _initial_mapping_end = .;
    stack_start = .;
    _bss_end = .;
    /DISCARD/ : {
         *(.comment)
         *(.eh_frame)
         *(.note.gnu.build-id)
    }
}
