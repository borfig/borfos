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
#include <scheduler.h>
#include <ctors.h>
#include <regs.h>
#include <mem.h>
#include <string.h>
#include <asm.h>
#include <timers.h>

list_t run_queue = LIST_INIT(run_queue);
unsigned tasks_in_run_queue;

task_t *current_task;
uint8_t scheduler_started;

#define STACK_PUSH(s, v) do {\
        (s) -= sizeof(v);     \
        *(typeof(v)*)(s) = (v);                 \
    } while(0)

#define STACK_PUSH_EMPTY(s, t) do {\
        (s) -= sizeof(t);          \
        memset((s), 0, sizeof(t)); \
    } while(0)

#define STACK_POP(s, v) do {\
        v = *typeof(v)s;    \
        s += sizeof(v);     \
    } while(0)

static void timer_callback(void *arg)
{
    schedule_task((task_t*)arg);
}

task_t *create_kernel_task(task_main_function_t func)
{
    task_t *new_task = malloc(sizeof(*new_task));
    new_task->kernel_stack_page = kernel_page_allocate(0);
    new_task->kernel_stack = new_task->kernel_stack_page + PAGE_SIZE;
    list_init(&new_task->queue_node);

    STACK_PUSH(new_task->kernel_stack, (uintptr_t)func);
    STACK_PUSH_EMPTY(new_task->kernel_stack, pushaed_registers_t);

    timer_init(&new_task->timer, 0, timer_callback, new_task);

    return new_task;
}

void schedule_task(task_t *task)
{
    if (list_is_empty(&task->queue_node)) {
        list_insert_before(&run_queue, &task->queue_node);
        ++tasks_in_run_queue;
    }
}

void unschedule_task(task_t *task)
{
    if (!list_is_empty(&task->queue_node)) {
        list_remove(&task->queue_node);
        --tasks_in_run_queue;
    }
}

static void idle(void)
{
    for (;;) {
        giveup_cpu();
        if (tasks_in_run_queue <= 1) {
            enable_interrupts();
            wait_for_interrupts();
            disable_interrupts();
        }
    }
}

CONSTRUCTOR(scheduler)
{
    current_task = start_kernel_task(idle);
}

task_t *next_task(void)
{
    current_task = LIST_ELEMENT_AT(run_queue.next, task_t, queue_node);
    return current_task;
}

__asm__(".globl scheduler_start\n"
        "scheduler_start:\n"
        "movb $1, scheduler_started\n"
        "mov current_task, %eax\n"
        "movl $1f, (%esp)\n"
        "ret\n"
        ".globl schedule\n"
        "schedule:\n"
        "pusha\n"
        "mov current_task, %eax\n"
        "mov %esp, (%eax)\n"
        "call next_task\n"
        "1:\n"
        "mov (%eax), %esp\n"
        "popa\n"
        "ret");

void giveup_cpu(void)
{
    list_remove(&current_task->queue_node);
    list_insert_before(&run_queue, &current_task->queue_node);
    schedule();
}

void unschedule(void)
{
    list_remove(&current_task->queue_node);
    --tasks_in_run_queue;
    schedule();
}

void sleep_tsc(uint64_t tsc)
{
    if (scheduler_started) {
        timer_register_relative(&current_task->timer, tsc);
        unschedule();
    }
    else {
        uint64_t dest = now() + tsc;
        while(now() < dest);
    }
}

void cancel_sleep(task_t *task)
{
    timer_unregister(&task->timer);
    schedule_task(task);
}
