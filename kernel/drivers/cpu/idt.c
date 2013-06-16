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
#include <idt.h>
#include <stdint.h>
#include <ctors.h>
#include <kprintf.h>

typedef struct {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
} __attribute__((packed)) IDT_entry_t;

typedef struct {
    uint16_t limit;
    IDT_entry_t *entries;
} __attribute__((packed)) IDT_t;

static IDT_entry_t idt[256];
const IDT_t IDT = {sizeof(idt)-1, idt};

static interrupt_handler_t handlers[256];

#define ISRE(n)                                 \
    __asm__ ("_isr" #n ": cli\n\t"              \
             "pushl $" #n "\n\t"                \
             "jmp isr_common\n\t");             \
    void _isr##n(void);

#define ISR(n)                                  \
    __asm__ ("_isr" #n ": cli\n\t"              \
             "push $0\n\t"                      \
             "pushl $" #n "\n\t"                \
             "jmp isr_common\n\t");             \
    void _isr##n(void);
#include "isr.h"
#undef ISR
#undef ISRE

/* Common handling code: */
__asm__ ("isr_common:\n"
         "pusha\n\t"
         "push %ds\n\t"
         "push %es\n\t"
         "push %fs\n\t"
         "push %gs\n\t"
         "movw $0x10, %ax\n\t"
         "movw %ax, %ss\n\t"
         "movw %ax, %ds\n\t"
         "movw %ax, %es\n\t"
         "movw %ax, %fs\n\t"
         "movw %ax, %gs\n\t"
         "mov %esp, %eax\n\t"
         "push %eax\n\t"
         "call isr_handler\n\t"
         "add $4, %esp\n\t"
         "pop %gs\n\t"
         "pop %fs\n\t"
         "pop %es\n\t"
         "pop %ds\n\t"
         "popa\n\t"
         "add $8, %esp\n\t"
         "iret\n\t");


static void set_gate(uint8_t index,
                     uint32_t offset)
{
    IDT_entry_t* e = idt + index;
    e->offset_1 = (uint16_t)offset;
    e->selector = 0x8;
    e->type_attr = 0x8e;
    e->offset_2 = (uint16_t)(offset >> 16);
}

CONSTRUCTOR(idt)
{
#define ISR(n) set_gate(n, (uint32_t)_isr##n);
#define ISRE(n) ISR(n)
#include "isr.h"
#undef ISRE
#undef ISR
}

static const char *exception_messages[0x20] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(int_regs_t *regs)
{
    interrupt_handler_t h = handlers[regs->int_no];
    if(h)
        (*h)(regs);
    else {
        kprintf("%s (%d) exception at %04x:%08x",
                (regs->int_no < 0x20) ? exception_messages[regs->int_no] : "Unknown", regs->int_no, regs->cs, regs->eip);
        kprintf("Err code %x eflags %08x", regs->err_code, regs->eflags);
        kprintf("eax %08x ecx %08x edx %08x ebx %08x", regs->eax, regs->ecx, regs->edx, regs->ebx);
        kprintf("esp %08x ebp %08x esi %08x edi %08x", regs->ret_esp, regs->ebp, regs->esi, regs->edi);
        kprintf("ds %04hx es %04hx fs %04hx gs %04hx", regs->ds, regs->es, regs->fs, regs->gs);
        PANIC("Unhandled exception");
    }
}

void set_interrupt_handler(uint8_t int_no, interrupt_handler_t handler, uint8_t type_attr)
{
    IDT_entry_t* e = idt + int_no;
    e->type_attr = type_attr;
    handlers[int_no] = handler;
}
