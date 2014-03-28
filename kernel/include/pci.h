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
#ifndef PCI_H
#define PCI_H

#include <list.h>

#include <stdint.h>

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;

    uint32_t reg00;
    uint32_t reg08;
    uint32_t bar[6];
    uint32_t reg3c;

    list_t by_class;

} pci_device_t;

pci_device_t *pci_get_by_location(unsigned bus, unsigned slot, unsigned function);

list_t *pci_get_devices_by_class(unsigned cl, unsigned sb);

static inline uint16_t pci_vendor_id(const pci_device_t *pci)
{
    return pci->reg00;
}

static inline uint16_t pci_device_id(const pci_device_t *pci)
{
    return pci->reg00 >> 16;
}

static inline uint8_t pci_class_code(const pci_device_t *pci)
{
    return pci->reg08 >> 24;
}

static inline uint8_t pci_subclass(const pci_device_t *pci)
{
    return pci->reg08 >> 16;
}

static inline uint8_t pci_progif(const pci_device_t *pci)
{
    return pci->reg08 >> 8;
}

uint16_t pci_command(pci_device_t *dev, uint16_t set, uint16_t unset);

#define PCI_COMMAND_IO		 0x001
#define PCI_COMMAND_MEMORY	 0x002
#define PCI_COMMAND_MASTER	 0x004
#define PCI_COMMAND_SPECIAL	 0x008
#define PCI_COMMAND_INVALIDATE	 0x010
#define PCI_COMMAND_VGA_PALETTE  0x020
#define PCI_COMMAND_PARITY	 0x040
#define PCI_COMMAND_WAIT 	 0x080
#define PCI_COMMAND_SERR	 0x100
#define PCI_COMMAND_FAST_BACK	 0x200
#define PCI_COMMAND_INTX_DISABLE 0x400


#endif /* PCI_H */
