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
#ifndef CPUID_H
#define CPUID_H

#include <stdint.h>

typedef struct {
    uint32_t features_ecx;
    uint32_t features_edx;
} cpuid_t;

extern cpuid_t cpuid;

#define CPUID(i, eax, ebx, ecx, edx) \
    __asm__ __volatile__("cpuid" \
                         : "=a"(eax), "=b"(ebx), \
                           "=c"(ecx), "=d"(edx) \
                         : "a"(i));

#define CPUIDA(i, eax) \
    __asm__ __volatile__("cpuid" \
                         : "=a"(eax) \
                         : "a"(i) \
                         : "ebx", "ecx", "edx" );

#endif /* CPUID_H */
