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
#include <ctors.h>
#include <asm.h>
#include <kprintf.h>

static int has_cpuid(void)
{
    uint32_t orig_flags = get_flags();
    set_flags(orig_flags ^ 0x200000);
    uint32_t new_flags = get_flags();
    return (new_flags & 0x200000) != (orig_flags & 0x200000);
}

#define CPUID(i, eax, ebx, ecx, edx) \
    __asm__ __volatile__("cpuid" \
                         : "=a"(eax), "=b"(ebx), \
                           "=c"(ecx), "=d"(edx) \
                         : "a"(i));

CONSTRUCTOR(cpuid)
{
    if (!has_cpuid()) {
        PANIC("Your CPU is too old: it lacks cpuid instruction");
    }
    uint32_t eax, ebx, ecx, edx;

    CPUID(0, eax, ebx, ecx, edx);
    if (ebx != 0x756e6547 ||
        edx != 0x49656e69 ||
        ecx != 0x6c65746e) {
        PANIC("Your CPU is not supported: it must be an Intel(r) one");
    }

    CPUID(1, eax, ebx, ecx, edx);

    if (!(edx & (1 << 4))) {
        PANIC("Your CPU is not supported: it does not have tsc");
    }
    if (!(edx & (1 << 5))) {
        PANIC("Your CPU is not supported: it does not suport MSRs");
    }

    if (!(ecx & (1 << 31))) {
        PANIC("Your CPU is not supported: it is not virtual");
    }
}
