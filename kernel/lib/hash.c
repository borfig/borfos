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
#include "hash.h"

/* hash_str() is based on FNV-1a.
 * http://isthe.com/chongo/tech/comp/fnv
 */

#define FNV_32_PRIME (0x01000193)

uint8_t hash_str(const char *str)
{
    uint32_t h = 0;
    while (*str) {
	h ^= (uint32_t)*(const unsigned char*)(str++);
	h *= FNV_32_PRIME;
    }
    return h >> 24;
}
