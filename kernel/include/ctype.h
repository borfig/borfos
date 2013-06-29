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
#ifndef CTYPE_H
#define CTYPE_H

#include <stdint.h>

#define	_U 0x01
#define	_L 0x02
#define	_N 0x04
#define	_S 0x08
#define _P 0x10
#define _C 0x20
#define _X 0x40
#define	_B 0x80

extern const uint8_t _ctype_flags[128];

static inline int _ctype_lookup(int c)
{
    return (0 <= c && c < 128) ? _ctype_flags[c] : 0;
}

#define CTYPE_FUNC_DEF(name, flags) \
    static inline int name(int c) { return _ctype_lookup(c) & (flags); }

CTYPE_FUNC_DEF(isalpha, _U|_L)
CTYPE_FUNC_DEF(isupper, _U)
CTYPE_FUNC_DEF(islower, _L)
CTYPE_FUNC_DEF(isdigit, _N)
CTYPE_FUNC_DEF(isxdigit, _X|_N)
CTYPE_FUNC_DEF(isspace, _S)
CTYPE_FUNC_DEF(ispunct, _P)
CTYPE_FUNC_DEF(isalnum, _U|_L|_N)
CTYPE_FUNC_DEF(isprint, _P|_U|_L|_N|_B)
CTYPE_FUNC_DEF(isgraph, _P|_U|_L|_N)
CTYPE_FUNC_DEF(iscntrl, _C)
CTYPE_FUNC_DEF(isblank, _B)

#undef CTYPE_FUNC_DEF

static inline int toupper(int c)
{
    return isupper(c) ? (c - 'A' + 'a') : c;
}

static inline int tolower(int c)
{
    return islower(c) ? (c - 'a' + 'A') : c;
}

#endif /* CTYPE_H */
