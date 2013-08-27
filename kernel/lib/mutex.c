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
#include <mutex.h>
#include <kprintf.h>

typedef struct {
    list_t node;
    task_t *task;
} waiter_t;

void mutex_lock(mutex_t *self)
{
    if (self->locker) {
        if (self->locker == current_task) {
            ++self->nesting;
            PANIC_IF(0 == self->nesting);
            return;
        }
        else {
            waiter_t waiter;
            list_insert_before(&self->waiters, &waiter.node);
            waiter.task = current_task;
            unschedule();
            list_remove(&waiter.node);
        }
    }
    self->locker = current_task;
    ++self->nesting;
    PANIC_IF(0 == self->nesting);
}

void mutex_unlock(mutex_t *self)
{
    PANIC_IF(self->locker != current_task);
    PANIC_IF(0 == self->nesting);
    --self->nesting;
    if(0 == self->nesting) {
        self->locker = NULL;
        if (!list_is_empty(&self->waiters)) {
            schedule_task(LIST_ELEMENT_AT(self->waiters.next, waiter_t, node)->task);
        }
    }
}
