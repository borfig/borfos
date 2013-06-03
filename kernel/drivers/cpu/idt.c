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
#include <idt.h>
#include <stdint.h>

typedef struct {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
} __attribute__((packed)) IDT_entry_t;

typedef struct {
    uint16_t limit;
    IDT_entry_t *entries;
} __attribute__((packed)) IDT_t;

static IDT_entry_t idt[256];
const IDT_t IDT = {sizeof(idt)-1, idt};

void idt_set_gate(uint8_t index,
                  uint32_t offset,
                  uint16_t selector,
                  uint8_t type_attr)
{
    IDT_entry_t* e = idt + index;
    e->offset_1 = (uint16_t)offset;
    e->selector = selector;
    e->type_attr = type_attr;
    e->offset_2 = (uint16_t)(offset >> 16);
}
