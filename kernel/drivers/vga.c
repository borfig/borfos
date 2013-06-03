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

static struct {
    unsigned cursor;
} VGA;

static void vga_sync_cursor_position(void)
{
    outb(VGA_IO_ADDRESS, 0xE);
    outb(VGA_IO_DATA, (uint8_t)(VGA.cursor >> 8));

    outb(VGA_IO_ADDRESS, 0xF);
    outb(VGA_IO_DATA, (uint8_t)(VGA.cursor));
}

static void vga_putchar(char c)
{
    switch (c) {
    case '\n':
        VGA.cursor = ((VGA.cursor / VGA_COLUMNS) + 1) * VGA_COLUMNS;
        break;
    case '\r':
        VGA.cursor -= VGA.cursor % VGA_COLUMNS;
        break;
    case '\b':
        if (VGA.cursor % VGA_COLUMNS) {
            --VGA.cursor;
            VGA_TEXT_MEMORY[VGA.cursor].character = ' ';
        }
        break;
    default:
        VGA_TEXT_MEMORY[VGA.cursor].character = c;
        VGA.cursor++;
        break;
    }

    if (VGA.cursor >= VGA_CELLS) {
        memmove(VGA_TEXT_MEMORY, VGA_TEXT_MEMORY + VGA_COLUMNS,
                (VGA_CELLS - VGA_COLUMNS) * sizeof(VGA_TEXT_MEMORY[0]));

        for(size_t i = VGA_CELLS - VGA_COLUMNS; i < VGA_CELLS; ++i)
            VGA_TEXT_MEMORY[i].character = ' ';

        VGA.cursor -= VGA_COLUMNS;
    }
}

static void vga_write(const char *buffer, size_t len)
{
    for(; len; ++buffer, --len)
        vga_putchar(*buffer);
    vga_sync_cursor_position();
}

static void kprintf_write(kprintf_backend_t * db, const char * buf, size_t len)
{
    db = db;
    vga_write(buf, len);
}

static kprintf_backend_t kprintf_backend = {
    .node = LIST_INIT(kprintf_backend.node),
    .kprintf_write = kprintf_write
};

CONSTRUCTOR(vga)
{
    VGA.cursor = 0;

    vga_sync_cursor_position();

    for(size_t i = 0; i < VGA_CELLS; ++i) {
        VGA_TEXT_MEMORY[i].character = ' ';
        VGA_TEXT_MEMORY[i].color = 0xF;
    }

    kprintf_register(&kprintf_backend);
}
