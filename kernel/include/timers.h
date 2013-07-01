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
#ifndef TIMERS_H
#define TIMERS_H

#include <list.h>

#include <clock.h>

typedef void (*timer_handler_t)(void*);

typedef struct {
    list_t node;
    uint64_t when;
    timer_handler_t func;
    void *arg;
} timer_t;

#define TIMER_INIT(var_name, when, func, arg) \
    { LIST_INIT(var_name.node), (when), (func), (arg) }

#define TIMER_RELATIVE_INIT(var_name, when, func, arg) \
    TIMER_INIT(var_name, now() + (when), (func), (arg))

static inline void timer_init(timer_t *self, uint64_t when, timer_handler_t func, void *arg)
{
    list_init(&self->node);
    self->when = when;
    self->func = func;
    self->arg = arg;
}

static inline void timer_relative_init(timer_t *self, uint64_t when, timer_handler_t func, void *arg)
{
    timer_init(self, now() + when, func, arg);
}

void timer_register(timer_t *);
void timer_unregister(timer_t *);

static inline bool timer_is_registered(timer_t *self)
{
    return !list_is_empty(&self->node);
}

void timer_interrupt_entry_point(void);

extern void (*timer_backend_set)(uint64_t);

#endif /* TIMERS_H */
