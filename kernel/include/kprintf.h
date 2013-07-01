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

#include <stddef.h>
#include <stdarg.h>

void kprintf(const char *format, /* format string */
             ...) __attribute__((format(printf, 1, 2)));

typedef struct kprintf_backend_s kprintf_backend_t;

typedef void (*kprintf_write_func_t)(kprintf_backend_t *, const char *, size_t);

struct kprintf_backend_s {
    list_t node;
    kprintf_write_func_t kprintf_write;
};

void kprintf_register(kprintf_backend_t *);

#define PANIC(text) do { \
        kprintf("PANIC at %s:%d:%s: %s", __FILE__, __LINE__, __func__, text); \
        freeze_system(); \
    } while(0)

#define ASSERT(condition) do {\
    if (!(condition)) \
        PANIC(#condition " failed"); \
    } while(0)

#endif /* KPRINTF_H */
