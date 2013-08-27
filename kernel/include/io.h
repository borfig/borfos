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
#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t v)
{
    __asm__ __volatile__("outb %0,%1" : : "a" (v), "dN" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t v;
    __asm__ __volatile__("inb %1,%0" : "=a" (v) : "dN" (port));
    return v;
}

static inline void outw(uint16_t port, uint16_t v)
{
    __asm__ __volatile__("outw %0,%1" : : "a" (v), "dN" (port));
}

static inline uint8_t inw(uint16_t port)
{
    uint16_t v;
    __asm__ __volatile__("inw %1,%0" : "=a" (v) : "dN" (port));
    return v;
}

static inline void outl(uint16_t port, uint32_t v)
{
    __asm__ __volatile__("outl %0,%1" : : "a" (v), "dN" (port));
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t v;
    __asm__ __volatile__("inl %1,%0" : "=a" (v) : "dN" (port));
    return v;
}

static inline void outsb(uint16_t port, const uint8_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; outsb"
                         : "+D"(out), "+c"(count) : "d"(port));
}

static inline void insb(uint16_t port, uint8_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; insb"
                         : "+D"(out), "+c"(count) : "d"(port));
}

static inline void outsw(uint16_t port, const uint16_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; outsw"
                         : "+D"(out), "+c"(count) : "d"(port));
}

static inline void insw(uint16_t port, uint16_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; insw"
                         : "+D"(out), "+c"(count) : "d"(port));
}

static inline void outsl(uint16_t port, const uint32_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; outsl"
                         : "+D"(out), "+c"(count) : "d"(port));
}

static inline void insl(uint16_t port, uint32_t *out, uint32_t count)
{
    __asm__ __volatile__("rep; insl"
                         : "+D"(out), "+c"(count) : "d"(port));
}

#endif /* IO_H */
