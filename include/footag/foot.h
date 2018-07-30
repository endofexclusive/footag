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

#ifndef FOOTAG_FOOT_H
#define FOOTAG_FOOT_H

#include <footag/footag.h>

enum {
        FOOT_OK,
        FOOT_FAIL,
};

struct geom_ctx;
struct foot_ctx;

struct foot_ctx *foot_open(
        const struct footag_item *tagitems
);

int foot_close(
        struct foot_ctx *ctx
);

int foot_calc(
        struct foot_ctx *ctx
);

int foot_gen(
        struct foot_ctx *ctx,
        struct geom_ctx *geom
);

int foot_getbounds(
        struct foot_ctx *ctx,
        double *w,
        double *h
);

/*
 * Allocate, generate and return a name describing the footprint. The user is
 * responsible for freeing the returned pointer.
 *
 * The name will be prefixed with the string in the prefix parameter. A default
 * prefix will be used if the prefix value is NULL. If the suffix parameter is
 * not NULL, then a '_' character and the suffix will be appended to the
 * generated name string.
 *
 * prefix: user string or NULL for default
 * suffix: user string or NULL for none
 */
char *foot_name(
        struct foot_ctx *ctx,
        const char *prefix,
        const char *suffix
);

static const double FOOT_GOLDEN_RATIO = 1.618033988749894848;

#endif

