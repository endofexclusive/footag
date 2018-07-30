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

#include <footag/ipc7351b.h>
#include "grid_priv.h"
#define dbg(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_DEBUG, \
        &ctx->two.foot.foolog, \
        __VA_ARGS__ \
)

struct bga_ctx {
        struct grid_ctx two;
        struct smdlead lead;
        enum ipcb_density density;
};

static int calcpads(struct foot_ctx *foot)
{
        struct bga_ctx *ctx = (struct bga_ctx *) foot;
        int ret;
        struct ipcb_bgaspec spec;

        ret = ipcb_get_bga(ctx->lead.width.nom, &spec);
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

        ctx->density = spec.density;
        ctx->two.foot.pad.w = spec.diam;
        if (0) {
                ctx->two.foot.pad.w = landsnap(&ctx->two.foot,
                    ctx->two.foot.pad.w);
        }
        ctx->two.foot.pad.h = ctx->two.foot.pad.w;

        return FOOT_OK;
}

static int dotagitems(struct bga_ctx *ctx, const struct footag_item *tagitems)
{
        const struct footag_item *ti;
        for (ti = tagitems; ti->tag != FOOTAG_END; ti++) {
                foot_grid_dotagitem(&ctx->two, ti);
                foot_smdlead_dotagitem(&ctx->lead, ti->tag, &ti->data);
        }
        return FOOT_OK;
}

static struct foot_ctx *init(
        struct foot_ctx *foot,
        const struct footag_item *tagitems
)
{
        struct bga_ctx *ctx = (struct bga_ctx *) foot;
        ctx->two.comp = GRID_COMP_DEFAULT;
        ctx->density = IPCB_DENSITY_N;
        dotagitems(ctx, tagitems);
        return foot;
}

static int fini(struct foot_ctx *foot)
{
        struct bga_ctx *ctx = (struct bga_ctx *) foot;
        foot_stringlist_free(ctx->two.comp.rownames);
        return FOOT_OK;
}

/*
 * example: BGA484C_P1.00_22X22_23.00X23.00X2.20_L0.60N_Artix-7
 */
static const char *NAME_TEMPLATE = "484C_P1.00_22x22_23.00x23.00x2.20_L0.60N_";

static char *doname(struct foot_ctx *foot, const char *pre, const char *suf)
{
        const int VERBOSE = 0;
        struct bga_ctx *ctx = (struct bga_ctx *) foot;
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

        if (VERBOSE) {
                act = snprintf(p, len, "C");
                p += act; len -= act;
        }

        act = snprintf(p, len, "_P%.2f_", ctx->two.comp.pitch);
        p += act; len -= act;

        if (VERBOSE) {
                act = snprintf(
                        p,
                        len,
                        "%dx%d_",
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
                ctx->lead.width.nom
        );
        p += act; len -= act;

        act = snprintf(p, len, "%c", IPCB_DENSITY_TO_CHAR[ctx->density]);
        p += act; len -= act;

        if (strlen(suf)) {
                act = snprintf(p, len, "_%s", suf);
                p += act; len -= act;
        }
        dbg(ctx, "name=<%s>\n", name);

        return name;
}

const struct foot_ops FOOT_OPS_BGA_IPCB = {
        .padspec        = { .type = GEOM_PAD_CIRC, .param = 0.1, },
        .calcpads       = calcpads,
        .drawpads       = foot_grid_drawpads,
        .setpad         = foot_setpad_default,
        .drawsilk       = foot_drawsilk_corners,
        .drawassy       = foot_drawgoldenassy,
        .calccyard      = foot_calccyard_ipcb,
        .drawcyard      = foot_drawcyard_ipcb,
        .name           = doname,
        .init           = init,
        .fini           = fini,
        .size           = sizeof(struct bga_ctx),
};

