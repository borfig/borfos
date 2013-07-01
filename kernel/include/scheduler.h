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
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <list.h>
#include <timers.h>

typedef struct task_s {
    char *kernel_stack; /* must be first! */
    char *kernel_stack_page;
    list_t queue_node;
} task_t;

extern task_t *current_task;

typedef void (*task_main_function_t)(void);

task_t *create_kernel_task(task_main_function_t func);

void schedule_task(task_t *t);
void unschedule_task(task_t *t);

static inline task_t *start_kernel_task(task_main_function_t func)
{
    task_t *t = create_kernel_task(func);
    schedule_task(t);
    return t;
}

void scheduler_start(void);

void unschedule(void);
void giveup_cpu(void);

void schedule();

void sleep_tsc(uint64_t tsc);

#endif /* SCHEDULER_H */
