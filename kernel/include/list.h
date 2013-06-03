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
#ifndef LIST_H
#define LIST_H

#include <containerof.h>
#include <stdbool.h>

typedef struct list_s {
    struct list_s *prev;
    struct list_s *next;
} list_t;

#define LIST_INIT(var_name) { &(var_name), &(var_name) }

static inline void list_init(list_t *self)
{
    self->prev = self;
    self->next = self;
}

static inline void list_insert_after(list_t *node,
                                     list_t *new_node)
{
    list_t *next = node->next;
    new_node->prev = node;
    new_node->next = next;
    next->prev = node->next = new_node;
}

static inline void list_insert_before(list_t *node,
                                      list_t *new_node)
{
    list_t *prev = node->prev;
    new_node->prev = prev;
    new_node->next = node;
    prev->next = node->prev = new_node;
}

static inline void list_remove(list_t *node)
{
    list_t *prev = node->prev;
    list_t *next = node->next;
    prev->next = next;
    next->prev = prev;
    node->prev = node->next = node;
}

static inline bool list_is_empty(list_t *self)
{
    return self->prev == self;
}

#define LIST_FOREACH(self, var_node) \
    for(var_node = (self)->next; var_node != (self); var_node = var_node->next)

#define LIST_FOREACH_REVERSE(self, var_node) \
    for(var_node = (self)->prev; var_node != (self); var_node = var_node->prev)

#define LIST_ELEMENT_AT(node, type, member) container_of(node, type, member)

#endif /* LIST_H */
