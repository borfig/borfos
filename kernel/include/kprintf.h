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
#ifndef KPRINTF_H
#define KPRINTF_H

#include <asm.h>
#include <list.h>
#include <file.h>
#include <stdarg.h>

void kprintf(const char *format, /* format string */
             ...) __attribute__((format(printf, 1, 2)));

void kprintf_register(file_t *);

#define PANIC(text) do { \
        kprintf("PANIC at %s:%d:%s: %s", __FILE__, __LINE__, __func__, text); \
        freeze_system(); \
    } while(0)

#define ASSERT(condition) do {\
    if (!(condition)) \
        PANIC(#condition " failed"); \
    } while(0)

#define PANIC_IF(expr) do {                     \
        if((expr))                              \
            PANIC(#expr " failed");             \
    } while(0)

#endif /* KPRINTF_H */
