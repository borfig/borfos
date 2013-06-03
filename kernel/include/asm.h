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
#ifndef ARCH_ASM_H
#define ARCH_ASM_H

#include <stdint.h>

static inline void disable_interrupts(void)
{
    __asm__ __volatile("cli");
}

static inline void enable_interrupts(void)
{
    __asm__ __volatile("sti");
}

static inline void wait_for_interrupts(void)
{
        __asm__ __volatile("hlt");
}

static inline void freeze_system(void)
{
    disable_interrupts();
    for (;;)
        wait_for_interrupts();
}

static inline uint32_t get_flags(void)
{
    uint32_t r;
    __asm__ __volatile__("pushf\n"
                         "pop %0\n"
                         : "=r"(r));
    return r;
}

static inline void set_flags(uint32_t r)
{
    __asm__ __volatile__("push %0\n"
                         "popf\n"
                         : : "r"(r) : "flags");
}

static inline uint64_t rdtsc(void)
{
    uint64_t r;
    __asm__ __volatile__("rdtsc" : "=A"(r));
    return r;
}

static inline void wrmsr64(uint32_t msr, uint64_t v)
{
    __asm__ __volatile__("wrmsr" : : "c"(msr), "A"(v));
}

static inline void wrmsr32(uint32_t msr, uint32_t v)
{
    __asm__ __volatile__("wrmsr" : : "c"(msr), "a"(v), "d"(0));
}

static inline uint64_t rdmsr64(uint32_t msr)
{
    uint64_t r;
    __asm__ __volatile__("rdmsr" : "=A"(r) : "c"(msr));
    return r;
}

static inline uint64_t rdmsr32(uint32_t msr)
{
    uint32_t r;
    __asm__ __volatile__("rdmsr" : "=a"(r) : "c"(msr) : "edx");
    return r;
}


#endif /* ARCH_ASM_H */
