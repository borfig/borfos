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
#ifndef BPRINTF_H
#define BPRINTF_H

#include <stdarg.h>
#include <stddef.h>

size_t bprintf(char *buffer, size_t max, const char *format,
               ...) __attribute__((format(printf, 3, 4)));

size_t vbprintf(char *buffer, size_t max, const char *format, va_list ap);

#endif /* BPRINTF_H */
