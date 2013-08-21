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
#include <pci.h>
#include <ctors.h>
#include <io.h>
#include <kprintf.h>
#include <array.h>

#include <stdlib.h>
#include <string.h>

#define MAX_SLOTS (32)
#define MAX_FUNCTIONS (8)
#define MAX_BUSES (256)

typedef struct {
    pci_device_t *functions[MAX_FUNCTIONS];
} pci_slot_t;

typedef struct {
    pci_slot_t *slots[MAX_SLOTS];
} pci_bus_t;

static pci_bus_t *buses[MAX_BUSES];

pci_device_t *pci_get_by_location(unsigned bus, unsigned slot, unsigned function)
{
    pci_bus_t *pci_bus = buses[bus];
    if (!pci_bus)
        return NULL;
    pci_slot_t *pci_slot = pci_bus->slots[slot];
    if (!pci_slot)
        return NULL;
    return pci_slot->functions[function];
}

static pci_device_t *allocate_device(unsigned bus, unsigned slot, unsigned func)
{
    if (!buses[bus]) {
        buses[bus] = calloc(1, sizeof(*(buses[bus])));
        if (!buses[bus])
            return NULL;
    }
    pci_bus_t *pci_bus = buses[bus];
    if (!pci_bus->slots[slot]) {
        pci_bus->slots[slot] = calloc(1, sizeof(*(pci_bus->slots[slot])));
        if (!pci_bus->slots[slot])
            return NULL;
    }
    pci_slot_t *pci_slot = pci_bus->slots[slot];
    if (!pci_slot->functions[func]) {
        pci_slot->functions[func] = calloc(1, sizeof(*(pci_slot->functions[func])));
    }

    return pci_slot->functions[func];
}

#define MAX_SUBCLASSES (256)
#define MAX_CLASSES (256)

typedef struct {
    list_t devices;
} pci_subclass_t;

typedef struct {
    pci_subclass_t subclasses[MAX_SUBCLASSES];
} pci_class_t;

static pci_class_t *classes[MAX_CLASSES];

static pci_subclass_t *allocate_subclass(unsigned class, unsigned subclass)
{
    if (!classes[class]) {
        classes[class] = malloc(sizeof(*classes[class]));
        if (!classes[class])
            return NULL;
        ARRAY_FOREACH(pci_subclass, classes[class]->subclasses) {
            list_init(&pci_subclass->devices);
        }
    }
    return classes[class]->subclasses + subclass;
}

list_t *pci_get_devices_by_class(unsigned cl, unsigned sb)
{
    if (!classes[cl])
        return NULL;
    return &classes[cl]->subclasses[sb].devices;
}

static uint32_t pciread32(unsigned bus, unsigned slot, unsigned func, unsigned offset)
{
    outl(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000);
    return inl(0xCFC);
}

static uint16_t pciread16(unsigned bus, unsigned slot, unsigned func, unsigned offset)
{
    uint32_t r = pciread32(bus, slot, func, offset);
    if (offset & 2)
        return r >> 16;
    return r;
}

static void pci_check_bus(unsigned bus);

static void pci_check_function(unsigned bus, unsigned slot, unsigned func)
{
    uint32_t dv = pciread32(bus, slot, func, 0);
    if ((dv & 0xFFFF) == 0xFFFF)
        return;
    uint32_t category = pciread32(bus, slot, func, 0x8);
    if ((category >> 16) == 0x604) { /* PCI-to-PCI bridge */
        uint32_t reg18 = pciread32(bus, slot, func, 0x18);
        pci_check_bus((reg18 >> 8) & 0xFF);
        return;
    }
    pci_device_t *device = allocate_device(bus, slot, func);
    if (NULL == device) {
        PANIC("not enough memory");
        return;
    }
    pci_subclass_t *subclass = allocate_subclass(category >> 24, (category >> 16) & 0xFF);
    if (NULL == subclass) {
        PANIC("not enough memory");
        return;
    }
    list_insert_before(&subclass->devices, &device->by_class);
    device->bus = bus;
    device->slot = slot;
    device->function = func;
    device->reg00 = dv;
    device->reg08 = category;
    for (int i = 0; i <=5; ++i) {
        device->bar[i] = pciread32(bus, slot, func, 0x10+i*4);
    }
    device->reg3c = pciread32(bus, slot, func, 0x3c);
}

static void pci_check_device(unsigned bus, unsigned slot)
{
    uint32_t dv = pciread32(bus, slot, 0, 0);
    if ((uint16_t)dv == 0xFFFF)
        return;
    unsigned functions = (pciread32(bus, slot, 0, 0xC) & 0x800000) ? 8 : 1;
    for(unsigned function = 0; function < functions; ++function)
        pci_check_function(bus, slot, function);
}

void pci_check_bus(unsigned bus)
{
    for(unsigned slot = 0; slot < 32; ++slot) {
        pci_check_device(bus, slot);
    }
}

CONSTRUCTOR(pci)
{
    if(pciread32(0, 0, 0, 0xC) & 0x800000) {
        /* multiple PCI buses... */
        for(unsigned function = 0; function < 8; ++function) {
            if (pciread16(0, 0, function, 0) != 0xFFFF)
                pci_check_bus(function);
        }
    }
    else {
        pci_check_bus(0);
    }
}
