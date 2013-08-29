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
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "list.h"
#include "array.h"
#include "hash.h"

typedef struct {
    list_t buckets[256];
} hashtable_t;

void hashtable_init(hashtable_t*);

static inline list_t *hashtable_get_bucket(hashtable_t *self, uint8_t key)
{
    return self->buckets + key;
}

static inline void hashtable_add_by_key(hashtable_t *self, list_t *node, uint8_t key)
{
    list_insert_before(hashtable_get_bucket(self, key), node);
}

static inline void hashtable_remove(list_t *node)
{
    list_remove(node);
}

static inline list_t *hashtable_find(hashtable_t *self, uint8_t key, list_predicate_t func, void *args)
{
    return list_find(hashtable_get_bucket(self, key), func, args);
}

#define HASHTABLE_FOREACH(self, node_var_name, bucket_var_name) \
    ARRAY_FOREACH(bucket_var_name, self->buckets) \
    LIST_FOREACH(bucket_var_name, node_var_name)

#endif /* HASHTABLE_H */
