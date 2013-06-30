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

DEPENDENCIES = {
    'idt':[],
    'vga':[],
    'mem':['idt'],
    'cpuid':['vga'],
    'lapic':['cpuid', 'mem','vga'],
    'kvm':['cpuid', 'lapic', 'vga'],
    'pic':['idt'],
    }
