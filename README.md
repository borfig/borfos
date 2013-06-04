# borfos

Yet another hobby OS for the x86 architecture.

Copyright (C) 2013 Boris Figovsky.

borfos is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

borfos is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with borfos.  If not, see http://www.gnu.org/licenses/.

## How to build
Requirements:
* [GCC](http://gcc.gnu.org/) for x86, that can build 32-bit code;
* [GNU Make](http://www.gnu.org/software/make/);
* [Python](http://www.python.org/) 2.7 (3.x are not supported).

In order to build, just run:
```sh
make
```
in the repository root directory.

## How to run

The kernel follows the [MultiBoot specification](http://www.gnu.org/software/grub/manual/multiboot/multiboot.html).

In order to run it on [KVM](http://www.linux-kvm.org/), execute:
```sh
qemu-system-x86_64 -kernel kernel/build/kernel.bin -cpu core2duo
```
or
```sh
qemu-system-i386 -kernel kernel/build/kernel.bin -cpu core2duo
```