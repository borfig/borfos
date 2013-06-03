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
#include <stdint.h>

typedef struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) GDT_entry_t;

typedef struct {
    uint16_t length;
    const GDT_entry_t *entries;
} __attribute__((packed)) GDT_t;

static GDT_entry_t GDT_entries[] = {
    {0, 0, 0, 0, 0, 0}, /* NULL */
    {0xFFFF, 0, 0, 0x9A, 0xCF, 0}, /* kernel code */
    {0xFFFF, 0, 0, 0x92, 0xCF, 0}, /* kernel data */
    {0xFFFF, 0, 0, 0xFA, 0xCF, 0}, /* user code */
    {0xFFFF, 0, 0, 0xF2, 0xCF, 0}, /* user data */
};

const GDT_t GDT = {
    sizeof(GDT_entries)-1,
    GDT_entries,
};
