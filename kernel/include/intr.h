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
#ifndef INTR_H
#define INTR_H

#include "idt.h"

typedef void (*interrupt_handler_t)(void);

static inline void intr_set(uint8_t index, interrupt_handler_t handler)
{
    idt_set_gate(index, (uint32_t)handler, 8, 0x8E);
}

#define INTR_NO_ERROR(name, c_handler)                       \
    void name(void);                                         \
    __asm__ (#name ": cli\n"                                 \
             "push %ds\n"                                    \
             "push %eax\n"                                   \
             "mov $0x10, %eax\n"                             \
             "mov %ax, %ds\n"                                \
             "call " #c_handler "\n"                         \
             "pop %eax\n"                                    \
             "pop %ds\n"                                     \
             "iret"                                          \
             )

#endif /* INTR_H */
