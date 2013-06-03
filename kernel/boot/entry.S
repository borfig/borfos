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

        .globl mboot_entry
mboot_entry:
        cli
        cld
        cmp $0x2badb002, %eax
        jne 2f
        mov $PA(stack_start), %esp
        mov %esp, %ebp
        push %ebx
        call initial_process_multiboot
        add $4, %esp
        call memory_setup_paging
        mov $PA(kernel_page_directory), %eax
        mov %eax, %cr3
        mov %cr0, %eax
        or $0x80000000, %eax
        mov %eax, %cr0
        lgdt GDT
        mov $0x10, %eax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        mov %ax, %ss
        ljmp $8, $1f
1:      lidt IDT
        mov $stack_start, %esp
        mov %esp, %ebp
        call memory_post_setup_paging
        call startup
2:      cli
1:      hlt
        jmp 1b
