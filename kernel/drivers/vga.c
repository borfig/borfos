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
#include <ctors.h>
#include <kprintf.h>
#include <io.h>
#include <string.h>

typedef struct {
    char character;
    uint8_t color;
} __attribute__((packed)) VGA_text_element_t;

#define VGA_TEXT_MEMORY ((VGA_text_element_t*)(0xc00b8000))

#define VGA_ROWS (25)
#define VGA_COLUMNS (80)
#define VGA_CELLS (VGA_ROWS * VGA_COLUMNS)

#define VGA_IO_ADDRESS (0x3D4)

#define VGA_IO_DATA (0x3D5)

typedef struct {
    file_t file;
    unsigned cursor;
} VGA_t;

static void vga_sync_cursor_position(VGA_t *vga)
{
    outb(VGA_IO_ADDRESS, 0xE);
    outb(VGA_IO_DATA, (uint8_t)(vga->cursor >> 8));

    outb(VGA_IO_ADDRESS, 0xF);
    outb(VGA_IO_DATA, (uint8_t)(vga->cursor));
}

static ssize_t vga_write_byte(file_t *file, uint8_t b)
{
    VGA_t *vga = container_of(file, VGA_t, file);
    switch (b) {
    case '\n':
        vga->cursor = ((vga->cursor / VGA_COLUMNS) + 1) * VGA_COLUMNS;
        break;
    case '\r':
        vga->cursor -= vga->cursor % VGA_COLUMNS;
        break;
    case '\b':
        if (vga->cursor % VGA_COLUMNS) {
            --vga->cursor;
            VGA_TEXT_MEMORY[vga->cursor].character = ' ';
        }
        break;
    default:
        VGA_TEXT_MEMORY[vga->cursor].character = b;
        vga->cursor++;
        break;
    }

    if (vga->cursor >= VGA_CELLS) {
        memmove(VGA_TEXT_MEMORY, VGA_TEXT_MEMORY + VGA_COLUMNS,
                (VGA_CELLS - VGA_COLUMNS) * sizeof(VGA_TEXT_MEMORY[0]));

        for(size_t i = VGA_CELLS - VGA_COLUMNS; i < VGA_CELLS; ++i)
            VGA_TEXT_MEMORY[i].character = ' ';

        vga->cursor -= VGA_COLUMNS;
    }
    vga_sync_cursor_position(vga);
    return 1;
}

static const file_ops_t vga_file_ops = {
    .write_byte = vga_write_byte,
    .write_string = file_default_write_string,
    .write_buffer = file_default_write_buffer
};

static VGA_t VGA = {
    .file = { .ops = &vga_file_ops }
};

CONSTRUCTOR(vga)
{
    vga_sync_cursor_position(&VGA);

    for(size_t i = 0; i < VGA_CELLS; ++i) {
        VGA_TEXT_MEMORY[i].character = ' ';
        VGA_TEXT_MEMORY[i].color = 0xF;
    }

    kprintf_register(&VGA.file);
}
