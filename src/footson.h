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

#ifndef FOOTSON_H
#define FOOTSON_H

#include <stdio.h>

enum {
        FOOTSON_OK,
        FOOTSON_FAIL,
        FOOTSON_IO,
};

struct footson_ctx;

int footson_init(
        struct footson_ctx *ctx
);
int footson_push(
        struct footson_ctx *ctx,
        const char *str
);
int footson_pop(
        struct footson_ctx *ctx,
        const char *str
);
int footson_openobj(
        struct footson_ctx *ctx
);
int footson_closeobj(
        struct footson_ctx *ctx
);
int footson_emptyobj(
        struct footson_ctx *ctx,
        const char *name
);
int footson_addstring(
        struct footson_ctx *ctx,
        const char *str
);
int footson_addmember(
        struct footson_ctx *ctx,
        const char *name
);
int footson_dostring(
        struct footson_ctx *ctx,
        const char *name,
        const char *str
);
int footson_dolong(
        struct footson_ctx *ctx,
        const char *name,
        long n
);
int footson_dobool(
        struct footson_ctx *ctx,
        const char *name,
        int val
);

/* This is no secret. */
struct footson_ctx {
        FILE *stream;
        unsigned int first;
        int level;
};

#endif

