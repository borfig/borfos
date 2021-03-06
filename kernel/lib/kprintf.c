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
#include <kprintf.h>
#include <bprintf.h>

static list_t backends = LIST_INIT(backends);

void kprintf_register(file_t *file)
{
    list_insert_before(&backends, &file->kprintf_node);
}

#define MAX_LENGTH (128)

void kprintf(const char *format, ...)
{
    if (list_is_empty(&backends))
        return;
    char buffer[MAX_LENGTH];
    va_list ap;
    va_start(ap, format);
    size_t l = vbprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);
    if (l < MAX_LENGTH)
        buffer[l++] = '\n';
    else
        buffer[MAX_LENGTH-1] = '\n';

    list_t *node;
    LIST_FOREACH(&backends, node) {
        file_t *file = LIST_ELEMENT_AT(node, file_t, kprintf_node);
        file->ops->write_buffer(file, (const uint8_t*)buffer, l);
    }
}
