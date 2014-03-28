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
#include <file.h>
#include <string.h>

ssize_t file_default_write_string(file_t *self, const char *str)
{
    return self->ops->write_buffer(self, (const uint8_t *)str, strlen(str));
}

ssize_t file_default_write_buffer(file_t *self, const uint8_t *buf, size_t len)
{
    ssize_t total = 0, r=0;
    for(; len && 0 < (r = self->ops->write_byte(self, *buf));
        ++buf, --len) {
        ++total;
    }
    if (0 > r)
        return r;
    return total;
}

size_t foo(void)
{
    return 5;
}
