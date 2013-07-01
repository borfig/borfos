# Copyright 2013 Boris Figovsky <borfig@gmail.com>

# This file is part of borfos.

# borfos is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# borfos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with borfos. If not, see <http://www.gnu.org/licenses/>.

CC=gcc

ARCH=$(shell uname -m)

GCC_TARGET=$(shell gcc -v 2>&1 | grep ^Target | cut -d" " -f2)
GCC_VERSION=$(shell gcc -v 2>&1 | grep "^gcc version" | cut -d" " -f3 | cut -d. -f-3)

ifeq ($(ARCH),x86_64)
        ARCH_DIR=32
        ARCHFLAG=-m32
else
        ARCH_DIR=.
        ARCHFLAG=
endif

GCC_LIB=/usr/lib/gcc/$(GCC_TARGET)/$(GCC_VERSION)/$(ARCH_DIR)/libgcc.a

INCLUDES := -Iinclude -Ibuild/include

COMMONFLAGS=-MD $(ARCHFLAG) $(INCLUDES)

CFLAGS := $(COMMONFLAGS) -march=i386 -std=gnu99 -O3 -nostdlib -pedantic -Wall -Wextra -ffunction-sections -fdata-sections

CFLAGS += '-D_AND=,' '-D_DEFUN(a,b,c)=a(c)' '-D_EXFUN(a,b)=a(b)' '-D_CONST=const'

ASFLAGS = $(COMMONFLAGS) -DASM

LDFLAGS := -nostdlib -static -Xlinker --build-id=none -Xlinker --sort-common
LDFLAGS += -Xlinker --gc-sections

O := build

all: $(O)/kernel.bin $(O)/kernel.dis

OBJS := boot/entry.o boot/mboot.o boot/startup.o

OBJS += drivers/cpu/gdt.o drivers/cpu/idt.o drivers/cpu/cpuid.o
OBJS += drivers/cpu/lapic.o drivers/cpu/kvm.o
OBJS += drivers/vga.o drivers/pic.o
OBJS += drivers/timers/apic.o

OBJS += lib/mem.o lib/timers.o lib/scheduler.o
OBJS += lib/kprintf.o lib/bprintf.o

OBJS += libc/heap.o libc/ctype.o
OBJS += libc/newlib/memmove.o libc/newlib/memcmp.o libc/newlib/strncpy.o
OBJS += libc/newlib/strcat.o libc/newlib/strcpy.o

OBJS := $(addprefix $(O)/, $(OBJS))

-include $(OBJS:.o=.d)

$(O)/kernel.o: $(OBJS) $(GCC_LIB)
	gcc $(ARCHFLAG) -nostdlib -r -o $@ $^

$(O)/%.ld: %.ld.S
	mkdir -p $(dir $@) && cpp -P -C $(INCLUDES) -DASM -o $@ $^

$(O)/include/ctorsprio.h: tools/dep2prio.py include/ctorsprio.py
	mkdir -p $(dir $@) && python $^ > $@ || rm -f $@

$(shell grep -l ctors.h `find -name \*.c | cut -c3-`) : $(O)/include/ctorsprio.h

%.elf: %.ld %.o
	gcc $(ARCHFLAG) $(LDFLAGS) -o $@ -T $^

%.dis: %.elf
	objdump -d $^ > $@

%.bin: %.elf
	objcopy -O binary -v $^ $@

$(O)/%.o: %.c
	mkdir -p $(dir $@) && $(CC) -c $(CFLAGS) -o $@ $<

$(O)/%.o: %.S
	echo $@ && mkdir -p $(dir $@) && $(CC) -c $(ASFLAGS) -o $@ $<

clean:
	rm -rf $(O)
