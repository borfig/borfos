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
#include <clock.h>
#include <asm.h>
#include <idt.h>
#include <timers.h>
#include <kprintf.h>

static void apic_timer_handler(int_regs_t *regs)
{
    regs = regs;
    lapic_eoi();
    timer_interrupt_entry_point();
}

static void apic_set_timer(uint64_t value)
{
    wrmsr64(0x6E0, value);
}

CONSTRUCTOR(apictimer)
{
    if (!lapic_base) {
        PANIC("no apic found");
    }
    if (!(cpuid.features_ecx & (1 << 24))) {
        PANIC("TSC deadline is not supported");
    }
    int timer_intno = allocate_interrupt();
    if (timer_intno < 0)
        PANIC("not enough interrupts");
    set_kernel_interrupt_handler(timer_intno, apic_timer_handler);
    lapic_write(0x320, 0x40000u|(uint32_t)timer_intno);

    timer_backend_set = apic_set_timer;
}
