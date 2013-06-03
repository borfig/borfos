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
#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_ELEMENT_SIZE(arr) (sizeof((arr)[0]))

#define ARRAY_ELEMENTS(arr) (sizeof(arr)/ARRAY_ELEMENT_SIZE(arr))

#define ARRAY_DYNAMIC_FOREACH(var_name, arr, count) \
    for(typeof(&(arr)[0]) var_name = (arr); \
        var_name < (arr) + (count); ++var_name)

#define ARRAY_FOREACH(var_name, arr) ARRAY_DYNAMIC_FOREACH(var_name, (arr), ARRAY_ELEMENTS(arr))

#endif /* ARRAY_H */
