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
#include <bios.h>
#include <io.h>

typedef struct {
    file_t file;
    uint16_t io_port;
} serial_t;

enum {
    DATA = 0,
    IE,
    DIV_LS = 0,
    DIV_MS,
    IIFIFO = 2,
    LINE_CONTROL,
    MODEM_CONTROL,
    LINE_STATUS,
    MODEM_STATUS,
    SCRATCH
};

static ssize_t serial_write_byte(file_t *file, uint8_t b)
{
    serial_t *self = container_of(file, serial_t, file);

    while(0 == (inb(self->io_port + LINE_STATUS) & 0x20));
    outb(self->io_port, b);

    return 1;
}

static const file_ops_t serial_file_ops = {
    .write_byte = serial_write_byte,
    .write_string = file_default_write_string,
    .write_buffer = file_default_write_buffer
};

#define SERIAL_INIT(i)                                                  \
    { .file = { .ops = &serial_file_ops} }

serial_t serial_ports[4] = {
    SERIAL_INIT(0), SERIAL_INIT(1), SERIAL_INIT(2), SERIAL_INIT(3)
};

static inline bool serial_exists(const serial_t* s)
{
    return 0 != s->io_port;
}

CONSTRUCTOR(serial)
{
    int i;
    for(i = 0; i < 4; ++i) {
        uint16_t io_port = BDA.com_ports[i];
        serial_ports[i].io_port = io_port;
        if (io_port) {
            outb(io_port + IE, 0); // disable interrupts
            outb(io_port + LINE_CONTROL, 0x80); // start set baud rate
            outb(io_port + DIV_LS, 1);
            outb(io_port + DIV_MS, 0);
            outb(io_port + LINE_CONTROL, 0x03); // 8 bits, no parity, one stop bit
            outb(io_port + IIFIFO, 0xc7); // Enable FIFO, clear them, with 14-byte threshold
            outb(io_port + MODEM_CONTROL, 4);
            outb(io_port + IE, 1);
        }
    }
    if (serial_exists(serial_ports))
        kprintf_register(&serial_ports->file);
}
