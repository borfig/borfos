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
#include <pic.h>
#include <ctors.h>
#include <io.h>
#include <idt.h>
#include <kprintf.h>

enum {
    PIC1_COMMAND = 0x20,
    PIC1_DATA,
    PIC2_COMMAND = 0xA0,
    PIC2_DATA,

    ICW1_INIT = 0x10,
    EOI = 0x20,

    ICW1_ICW4 = 0x1,

    ICW4_8086 = 0x1
};

static uint8_t irq_mask[2] = {0xFF, 0xFF};

static pic_handler_t handlers[0x10];

static void pic_handler(int_regs_t *regs)
{
    uint8_t irq = regs->int_no - 0x20;
    if(irq >= 8)
        outb(PIC2_COMMAND, EOI); /* send EOI to PIC2 */
    outb(PIC1_COMMAND, EOI); /* send EOI to PIC1 */
    pic_handler_t handler = handlers[irq];
    if (handler)
        handler();
    else {
        kprintf("Unhandled PIC IRQ %d at %04x:%08x",
                irq, regs->cs, regs->eip);
        kprintf("eflags %08x", regs->eflags);
        kprintf("eax %08x ecx %08x edx %08x ebx %08x", regs->eax, regs->ecx, regs->edx, regs->ebx);
        kprintf("esp %08x ebp %08x esi %08x edi %08x", regs->ret_esp, regs->ebp, regs->esi, regs->edi);
        kprintf("ds %04hx es %04hx fs %04hx gs %04hx", regs->ds, regs->es, regs->fs, regs->gs);
        PANIC("Unhandled PIC IRQ");
    }
}

static void set_mask(void)
{
    outb(PIC1_DATA, irq_mask[0]);
    outb(PIC2_DATA, irq_mask[1]);
}

CONSTRUCTOR(pic)
{
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    set_mask();
    int base = allocate_interrupts(4);
    if (base < 0)
        PANIC("could not allocate interrupts for PIC");
    for(uint8_t int_no = 0x20; int_no < 0x30; ++int_no)
        set_kernel_interrupt_handler(int_no, pic_handler);
}

void pic_set_handler(uint8_t irq, pic_handler_t handler)
{
    handlers[irq] = handler;
}

void pic_unmask(uint8_t irq)
{
    if (irq < 8) {
        irq_mask[0] &= ~(1 << (irq & 7));
    }
    else {
        irq_mask[1] &= ~(1 << (irq & 7));
        if (irq_mask[1] != 0xFF)
            irq_mask[0] &= ~(1 << 2);
    }
    set_mask();
}

void pic_mask(uint8_t irq)
{
    if (irq < 8) {
        irq_mask[0] |= 1 << (irq & 7);
    }
    else {
        irq_mask[1] |= 1 << (irq & 7);
        if (irq_mask[1] == 0xFF)
            irq_mask[0] |= 1 << 2;
    }
    set_mask();
}
