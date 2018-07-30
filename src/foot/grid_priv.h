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

#ifndef GRID_PRIV_H
#define GRID_PRIV_H

#include "foot_priv.h"

/* Component specification. Independent of land pattern policy. */
struct grid_comp {
        double pitch;
        int rows;
        int cols;
        /* Number of perimeter rows and cols. These will not get pads. */
        int prows;
        int pcols;
        char **rownames;
};

static const struct grid_comp GRID_COMP_DEFAULT = {
        .pitch = 2.0,
        .rows  = 8, .cols  = 8,
        .prows = 0, .pcols = 8,
        .rownames = NULL,
};

struct grid_ctx {
        struct foot_ctx foot;
        /* from user */
        struct grid_comp comp;
};

static inline struct foot_coord *extrpad(struct grid_ctx *ctx) {
        return &ctx->foot.extreme.pad;
}

int foot_grid_drawpads(
        struct foot_ctx *ctx,
        struct geom_ctx *g
);
int foot_grid_dotagitem(
        struct grid_ctx *ctx,
        const struct footag_item *ti
);

#endif

