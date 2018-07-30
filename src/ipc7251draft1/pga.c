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

#include <footag/ipc7251draft1.h>
#include "grid_priv.h"
#define dbg(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_DEBUG, \
        &ctx->two.foot.foolog, \
        __VA_ARGS__ \
)

struct pga_ctx {
        struct grid_ctx two;
        struct footol diam;
        enum ipc7251_density density;
};

static int calcpads(struct foot_ctx *foot)
{
        struct pga_ctx *ctx = (struct pga_ctx *) foot;
        int ret;
        struct ipc7251_spec spec;

        ret = ipc7251_get_pga(ctx->diam.max, ctx->density, &spec);
        if (ret) {
                return FOOT_FAIL;
        }
        dbg(ctx, "%s\n", spec.desc);

        if (!ctx->two.foot.cfg.cyexc) {
                ctx->two.foot.cfg.cyexc = spec.cyexc;
        }
        if (!ctx->two.foot.cfg.cyard.grid) {
                ctx->two.foot.cfg.cyard.grid = spec.round;
        }

        ctx->two.foot.pad.holew = spec.holediam;
        ctx->two.foot.pad.w = spec.paddiam;
        if (0) {
                ctx->two.foot.pad.holew = landsnap(&ctx->two.foot,
                    ctx->two.foot.pad.holew);
                ctx->two.foot.pad.w = landsnap(&ctx->two.foot,
                    ctx->two.foot.pad.w);
        }
        ctx->two.foot.pad.holeh = ctx->two.foot.pad.holew;
        ctx->two.foot.pad.h = ctx->two.foot.pad.w;

        return FOOT_OK;
}

static const enum ipc7251_density TO_IPC7251_DENSITY[FOOTAG_LEVEL_NUM] = {
        [FOOTAG_LEVEL_M] = IPC7251_DENSITY_M,
        [FOOTAG_LEVEL_N] = IPC7251_DENSITY_N,
        [FOOTAG_LEVEL_L] = IPC7251_DENSITY_L,
};

static int dotagitems(struct pga_ctx *ctx, const struct footag_item *tagitems)
{
        const struct footag_item *ti;
        for (ti = tagitems; ti->tag != FOOTAG_END; ti++) {
                foot_grid_dotagitem(&ctx->two, ti);
                if (ti->tag == FOOTAG_LEAD_DIAM) {
                        ctx->diam = ti->data.t;
                } else if (ti->tag == FOOTAG_DENSITY) {
                        ctx->density = TO_IPC7251_DENSITY[ti->data.i];
                }
        }
        return FOOT_OK;
}

static struct foot_ctx *init(
        struct foot_ctx *foot,
        const struct footag_item *tagitems
)
{
        struct pga_ctx *ctx = (struct pga_ctx *) foot;
        ctx->two.comp = GRID_COMP_DEFAULT;
        ctx->density = IPC7251_DENSITY_N;
        dotagitems(ctx, tagitems);
        return foot;
}

static int fini(struct foot_ctx *foot)
{
        struct pga_ctx *ctx = (struct pga_ctx *) foot;
        foot_stringlist_free(ctx->two.comp.rownames);
        return FOOT_OK;
}

/*
 * example: PGA224_P2.54_18x18_47.24x47.24x3.17_Ln.nnB_internet
 */
static const char *NAME_TEMPLATE = "224_P2.54_18x18_47.24x47.24x3.17_Ln.nnB_";

static char *doname(struct foot_ctx *foot, const char *pre, const char *suf)
{
        const int VERBOSE = 0;
        struct pga_ctx *ctx = (struct pga_ctx *) foot;
        char *name;
        char *p;
        size_t len;
        size_t act;

        len = 1 + strlen(pre) + strlen(suf);
        len += strlen(NAME_TEMPLATE);
        name = malloc(len);
        if (!name) {
                return NULL;
        }
        p = name;
        act = snprintf(p, len, "%s", pre);
        p += act; len -= act;

        {
                int npins = (
                        ctx->two.comp.rows * ctx->two.comp.cols -
                        ctx->two.comp.prows * ctx->two.comp.pcols
                );
                act = snprintf(p, len, "%d",npins);
                p += act; len -= act;
        }

        act = snprintf(p, len, "_P%.2f_", ctx->two.comp.pitch);
        p += act; len -= act;

        if (VERBOSE) {
                act = snprintf(
                        p,
                        len,
                        "%dx%d",
                        ctx->two.comp.rows,
                        ctx->two.comp.cols
                );
                p += act; len -= act;
                act = foot_bodyprint_wlh(&ctx->two.foot.body, p, len);
                p += act; len -= act;
        } else {
                act = foot_bodyprint_wl(&ctx->two.foot.body, p, len);
                p += act; len -= act;
        }

        act = snprintf(
                p,
                len,
                "_L%.2f",
                ctx->diam.nom
        );
        p += act; len -= act;

        act = snprintf(p, len, "%c", IPC7251_DENSITY_TO_CHAR[ctx->density]);
        p += act; len -= act;

        if (strlen(suf)) {
                act = snprintf(p, len, "_%s", suf);
                p += act; len -= act;
        }
        dbg(ctx, "name=<%s>\n", name);

        return name;
}

const struct foot_ops FOOT_OPS_PGA_IPC = {
        .padspec        = { .type = GEOM_PAD_TCIRC, },
        .calcpads       = calcpads,
        .drawpads       = foot_grid_drawpads,
        .setpad         = foot_setpad_th,
        .drawsilk       = foot_drawsilk_corners,
        .drawassy       = foot_drawgoldenassy,
        .calccyard      = foot_calccyard_ipcb,
        .drawcyard      = foot_drawcyard_ipcb,
        .name           = doname,
        .init           = init,
        .fini           = fini,
        .size           = sizeof(struct pga_ctx),
};

