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

#ifndef TWOSIDE_PRIV_H
#define TWOSIDE_PRIV_H

#include <footag/ipc7351b.h>
#include "tworow_priv.h"

#define dbg(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_DEBUG, \
        &ctx->two.foot.foolog, \
        __VA_ARGS__ \
)

struct soic_ctx {
        struct tworow_ctx two;
        struct smdlead lead;
        enum ipcb_density density;
};

static const enum ipcb_density FOOT_LEVEL_TO_IPCB_DENSITY[FOOTAG_LEVEL_NUM] = {
        [FOOTAG_LEVEL_M] = IPCB_DENSITY_M,
        [FOOTAG_LEVEL_N] = IPCB_DENSITY_N,
        [FOOTAG_LEVEL_L] = IPCB_DENSITY_L,
};

int foot_twoside_ipcb_calcpads(
        struct foot_ctx *foot,
        enum ipcb_leadtype leadtype
);

char *foot_twoside_ipcb_chip_name(
        struct foot_ctx *foot,
        const char *pre,
        const char *suf
);

char *foot_twoside_ipcb_soic_name(
        struct foot_ctx *foot,
        const char *pre,
        const char *suf
);

#endif

