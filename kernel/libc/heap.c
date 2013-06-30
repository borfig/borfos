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
#include <asm.h>
#include <mem.h>

#define HAVE_MMAP (1)

static void *__mmap__(size_t s)
{
    size_t pages = s >> PAGE_BITS;
    unsigned order = 0;
    for(; pages > (1u << order); ++order);
    void *result = kernel_page_allocate(order);
    if (NULL == result)
        return (void*)-1;
    return result;
}

static int __munmap__(void *addr, size_t s)
{
    s=s;
    kernel_page_free(addr);
    return 0;
}

#define MMAP(s) __mmap__((s))
#define DIRECT_MMAP(s) MMAP(s)
#define MUNMAP(a, s) __munmap__((a), (s))

#define LACKS_UNISTD_H
#define LACKS_FCNTL_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H
#define LACKS_STRINGS_H
#define LACKS_SYS_TYPES_H
#define LACKS_ERRNO_H
#define LACKS_SCHED_H
#define LACKS_TIME_H

#define MALLOC_FAILURE_ACTION

#define EINVAL (-1)
#define ENOMEM (-1)

#define ABORT freeze_system()

#define HAVE_MORECORE (0)
#define HAVE_MREMAP (0)
#define MAP_ANONYMOUS
#define NO_MALLOC_STATS (1)
#include <stddef.h>

#include "malloc/malloc.c"
