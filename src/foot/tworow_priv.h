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

#ifndef TWOROW_PRIV_H
#define TWOROW_PRIV_H

#include "foot_priv.h"

/* Component specification. Independent of land pattern policy. */
struct tworow_comp {
        double pitch;
        int nleads;
        /* component is polarized */
        int pol;
};

static const struct tworow_comp TWOROW_COMP_DEFAULT = {
        .pitch = 2.0, .nleads = 6, .pol = 1,
};
static const struct tworow_comp TWOROW_COMP_DEFAULT_2PIN = {
        .pitch = 0.0, .nleads = 2, .pol = 0,
};

struct tworow_ctx {
        struct foot_ctx foot;
        /* from user */
        struct tworow_comp comp;
        /* x offset from origin to pad center */
        double padcenter;
};

static inline struct foot_coord *extrpad(struct tworow_ctx *ctx) {
        return &ctx->foot.extreme.pad;
}
static inline struct foot_coord *extrsilk(struct tworow_ctx *ctx) {
        return &ctx->foot.extreme.silk;
}

int foot_tworow_drawpads(
        struct foot_ctx *ctx,
        struct geom_ctx *g
);
int foot_tworow_drawsilk(
        struct foot_ctx *ctx,
        struct geom_ctx *g
);
int foot_tworow_drawassy_golden_or_rect(
        struct foot_ctx *foot,
        struct geom_ctx *g
);
int foot_tworow_dotagitem(
        struct tworow_ctx *ctx,
        const struct footag_item *ti
);

#endif

