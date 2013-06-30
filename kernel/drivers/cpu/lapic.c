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
#include <lapic.h>
#include <cpuid.h>
#include <ctors.h>
#include <kprintf.h>
#include <asm.h>
#include <mem.h>

uint32_t lapic_base;

void default_lapic_eoi(void)
{
    _lapic_eoi();
}

void (*lapic_eoi)(void) = default_lapic_eoi;

CONSTRUCTOR(lapic)
{
    if (!(cpuid.features_edx & (1 << 9))) {
        kprintf("no APIC chip found");
        return;
    }
    uint32_t lapic_phaddr;
    uint32_t v = rdmsr32(0x1B);
    if (!(v & (1 << 11))) {
        wrmsr32(0x1B, v | (1 << 11));
    }
    lapic_phaddr = v & ~(PAGE_SIZE-1);

    lapic_base = (uint32_t)map_phaddr_in_kernel(lapic_phaddr, 0);
    lapic_write(0xE0, 0xFFFFFFFF);
}
