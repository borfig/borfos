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
#ifndef LAPIC_H
#define LAPIC_H

#include <stdint.h>

extern uint32_t lapic_base;

static inline uint32_t lapic_read(uint32_t reg)
{
    return *(uint32_t volatile*)(lapic_base + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value)
{
    *(uint32_t volatile*)(lapic_base + reg) = value;
}

static inline void _lapic_eoi(void)
{
    lapic_write(0xB0, 0);
}

extern void (*lapic_eoi)(void);

#endif /* LAPIC_H */
