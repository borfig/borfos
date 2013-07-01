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
#include <timers.h>
#include <kprintf.h>

static list_t timers = LIST_INIT(timers);

void (*timer_backend_set)(uint64_t) = NULL;

void timer_unregister(timer_t *timer)
{
    ASSERT(timer_backend_set);

    if (timer_is_registered(timer)) {
        list_remove(&timer->node);

        timer_backend_set(list_is_empty(&timers) ? 0 :
                          LIST_ELEMENT_AT(timers.next, timer_t, node)->when);
    }
}

void timer_register(timer_t *timer)
{
    ASSERT(timer_backend_set);

    if (timer_is_registered(timer))
        return;

    list_t *node;
    LIST_FOREACH(&timers, node) {
        if (timer->when < LIST_ELEMENT_AT(node, timer_t, node)->when)
            break;
    }

    list_insert_before(node, &timer->node);

    timer_backend_set(LIST_ELEMENT_AT(timers.next, timer_t, node)->when);
}

void timer_interrupt_entry_point(void)
{
    while (!list_is_empty(&timers)) {
        timer_t *timer = LIST_ELEMENT_AT(timers.next, timer_t, node);
        if (now() < timer->when) {
            timer_backend_set(timer->when);
            return;
        }
        list_remove(&timer->node);
        timer->func(timer->arg);
    }
    timer_backend_set(0);
}
