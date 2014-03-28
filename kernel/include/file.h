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
#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdtypes.h>
#include <list.h>

typedef struct file_s file_t;

typedef struct {
/* a file stream, write: */
    ssize_t (*write_byte)(file_t *, uint8_t);
    ssize_t (*write_string)(file_t *, const char*);
    ssize_t (*write_buffer)(file_t *, const uint8_t *, size_t);
} file_ops_t;

struct file_s {
    const file_ops_t *ops;
    list_t kprintf_node;
    size_t size;
};

ssize_t file_default_write_string(file_t *self, const char *str);
ssize_t file_default_write_buffer(file_t *self, const uint8_t *buf, size_t len);

#endif /* FILE_H */
