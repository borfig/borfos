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
#ifndef CLOCK_H
#define CLOCK_H

#include <asm.h>

uint64_t tsc_to_nanoseconds(uint64_t tsc);

static inline uint64_t now(void)
{
    return rdtsc();
}

uint64_t now_in_nanoseconds(void);

uint64_t nanoseconds_to_tsc(uint64_t ns);

#endif /* CLOCK_H */
