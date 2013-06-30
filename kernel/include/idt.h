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
#ifndef ARCH_IDT_H
#define ARCH_IDT_H

#include <stdint.h>

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
    uint32_t ret_esp; /* valid if cs != 8 */
} int_regs_t;

typedef void (*interrupt_handler_t)(int_regs_t*);

void set_interrupt_handler(uint8_t int_no, interrupt_handler_t handler, uint8_t type_attr);

static inline void set_kernel_interrupt_handler(uint8_t int_no, interrupt_handler_t handler)
{
    set_interrupt_handler(int_no, handler, 0x8e);
}

int allocate_interrupts(uint8_t order);

static inline int allocate_interrupt(void)
{
    return allocate_interrupts(0);
}

void print_int_regs(const int_regs_t *regs);

#endif /* ARCH_IDT_H */
