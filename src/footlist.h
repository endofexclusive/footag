/*
 * Copyright 2018 Martin Åberg
 *
 * This file is part of Footag.
 *
 * Footag is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Footag is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FOOTLIST_H
#define FOOTLIST_H

/* Linked list */

#include <stddef.h>

struct footnode {
        struct footnode *next;
};

struct footlist {
        struct footnode *head;
};

static inline void footlist_init(
        struct footlist *list
)
{
        list->head = NULL;
}

static inline void footlist_add(
        struct footlist *list,
        struct footnode *node
)
{
        node->next = list->head;
        list->head = node;
}

static inline struct footnode *footlist_rem(
        struct footlist *list
)
{
        struct footnode *node = list->head;
        if (node) {
                list->head = node->next;
                node->next = NULL;
        }
        return node;
}

#endif

