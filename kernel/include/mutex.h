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
#ifndef MUTEX_H
#define MUTEX_H

#include <scheduler.h>

typedef struct {
    task_t *locker;
    uint32_t nesting;
    list_t waiters;
} mutex_t;

#define MUTEX_INIT(var) {NULL, 0, LIST_INIT(var.waiters)}

static inline void mutex_init(mutex_t* self)
{
    self->locker = NULL;
    self->nesting = 0;
    list_init(&self->waiters);
}

void mutex_lock(mutex_t*);
void mutex_unlock(mutex_t*);

#endif /* MUTEX_H */
