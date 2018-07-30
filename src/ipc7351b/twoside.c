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

#include "twoside_priv.h"

int foot_twoside_ipcb_calcpads(
        struct foot_ctx *foot,
        enum ipcb_leadtype leadtype
)
{
        struct soic_ctx *ctx = (struct soic_ctx *) foot;
        int ret;
        struct ipcb_comp comp;
        struct ipcb_attr attr;
        struct ipcb_twospec spec;

        comp.lmin = ctx->lead.span.min;
        comp.lmax = ctx->lead.span.max;
        comp.tmin = ctx->lead.length.min;
        comp.tmax = ctx->lead.length.max;
        comp.wmin = ctx->lead.width.min;
        comp.wmax = ctx->lead.width.max;
        comp.pitch = ctx->two.comp.pitch;

        attr.density = ctx->density;
        attr.f = ctx->two.foot.cfg.f;
        attr.p = ctx->two.foot.cfg.p;

        ret = ipcb_get_two(leadtype, &comp, &attr, &spec);
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

        ctx->two.padcenter = (spec.zmax + spec.gmin) / 4;
        ctx->two.padcenter = landsnap(&ctx->two.foot, ctx->two.padcenter);
        ctx->two.foot.pad.h = spec.xmax;
        ctx->two.foot.pad.w = (spec.zmax - spec.gmin) / 2;
        if (0) {
                ctx->two.foot.pad.h = landsnap(&ctx->two.foot,
                    ctx->two.foot.pad.h);
                ctx->two.foot.pad.w = landsnap(&ctx->two.foot,
                    ctx->two.foot.pad.w);
        }

        return FOOT_OK;
}

/*
 * example:  RESC1.61x0.82x0.53_L0.30x0.40N_katten
 * example: CAPMP7.30x4.30x2.80_L1.30x2.40N_fisken
 */
static const char *CHIP_TEMPLATE = "n7.31xn4.32xn2.83_Ln1.34xn1.56N_";

char *foot_twoside_ipcb_chip_name(
        struct foot_ctx *foot,
        const char *pre,
        const char *suf
)
{
        const int VERBOSE = 0;
        struct soic_ctx *ctx = (struct soic_ctx *) foot;
        char *name;
        char *p;
        size_t len;
        size_t act;

        len = 1 + strlen(pre) + strlen(suf);
        len += strlen(CHIP_TEMPLATE);
        name = malloc(len);
        if (!name) {
                return NULL;
        }
        p = name;
        act = snprintf(p, len, "%s", pre);
        p += act; len -= act;

        if (VERBOSE) {
                act = foot_bodyprint_wlh(&ctx->two.foot.body, p, len);
        } else {
                act = foot_bodyprint_wl(&ctx->two.foot.body, p, len);
        }
        p += act; len -= act;

        act = snprintf(
                p,
                len,
                "_L%.2fx%.2f",
                ctx->lead.length.nom,
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

/*
 * example: SOIC8_P1.27_4.90X3.90X1.75_L0.83X0.41N_CDCVF2505
 */
static const char *SOIC_TEMPLATE =
    "nnn_Pn1.27_n5.28Xn5.28Xn1.96_Ln0.64Xn0.42N_";

char *foot_twoside_ipcb_soic_name(
        struct foot_ctx *foot,
        const char *pre,
        const char *suf
)
{
        const int VERBOSE = 0;
        struct soic_ctx *ctx = (struct soic_ctx *) foot;
        char *name;
        char *p;
        size_t len;
        size_t act;

        len = 1 + strlen(pre) + strlen(suf);
        len += strlen(SOIC_TEMPLATE);
        name = malloc(len);
        if (!name) {
                return NULL;
        }
        p = name;
        act = snprintf(p, len, "%s", pre);
        p += act; len -= act;

        {
                int npins = ctx->two.comp.nleads - foot_nskip(&ctx->two.foot);
                act = snprintf(p, len, "%d", npins);
                p += act; len -= act;
        }

        act = snprintf(p, len, "_P%.2f_", ctx->two.comp.pitch);
        p += act; len -= act;

        if (VERBOSE) {
                act = foot_bodyprint_wlh(&ctx->two.foot.body, p, len);
        } else {
                act = foot_bodyprint_wl(&ctx->two.foot.body, p, len);
        }
        p += act; len -= act;

        act = snprintf(
                p,
                len,
                "_L%.2fx%.2f",
                ctx->lead.length.nom,
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

