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

#include "tworow_priv.h"

#define dbg(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_DEBUG, \
        &ctx->foot.foolog, \
        __VA_ARGS__ \
)
#define err(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_ERROR, \
        &ctx->foot.foolog, \
        __VA_ARGS__ \
)

static inline double padxpos(const struct tworow_ctx *ctx, int col)
{
        assert(col == 0 || col == 1);
        double x;
        x = (double []) {-1, 1}[col];
        /* NOTE: already snapped */
        x *= ctx->padcenter;
        return x;
}

static inline double padypos(const struct tworow_ctx *ctx, int row)
{
        assert(0 <= row && 2 * row <= ctx->comp.nleads);
        int rows = ctx->comp.nleads / 2;
        double pitch = ctx->comp.pitch;
        double y;
        y = (rows / 2.0) - 1.0 / 2 - row;
        y *= pitch;
        /* tight pitch needs high pad center resolution */
        if (0.70 <= pitch) {
                y = landsnap(&ctx->foot, y);
        }
        return y;
}

/*
 * NOTE: This should be independent of pad size, package, standard, etc.
 * NOTE: This can be specific to tworow footprints.
 */
int foot_tworow_drawpads(struct foot_ctx *foot, struct geom_ctx *g)
{
        struct tworow_ctx *ctx = (struct tworow_ctx *) foot;
        int pinnum = 0;
        int rows = ctx->comp.nleads / 2;
        char **pinnames = ctx->foot.pinnames;
        int ndrawn = 0;
        int rowstart = 0;
        int rowend = rows;
        int rowadd = 1;

        for (int col = 0; col < 2; col++) {
                double x;
                x = padxpos(ctx, col);
                for (int row = rowstart; row != rowend; row += rowadd) {
                        double y;
                        char buf[PINNAME_BUFSIZE];
                        char *name = &buf[0];
                        int emit = 1;
                        if (pinnames) {
                                name = pinnames[pinnum];
                                emit = strcmp(name, "-");
                        } else {
                                int ret;
                                ret = snprintf(buf, NELEM(buf), "%d", pinnum+1);
                                if ((int) NELEM(buf) <= ret) {
                                        return FOOT_FAIL;
                                }
                        }
                        if (emit) {
                                y = padypos(ctx, row);
                                geom_moveto(g, x, y);
                                dbg(ctx, "pad %4s at %f, %f\n", name, x, y);
                                ctx->foot.ops->setpad(&ctx->foot, g, ndrawn);
                                geom_padc(g, name);
                                ndrawn++;
                        }
                        pinnum++;
                }
                rowstart = rows-1;
                rowend = -1;
                rowadd = -1;
        }
        extrpad(ctx)->x = fabs(padxpos(ctx, 0)) + ctx->foot.pad.w / 2;
        extrpad(ctx)->y = padypos(ctx, 0) + ctx->foot.pad.h / 2;

        return FOOT_OK;
}

int foot_tworow_drawsilk(struct foot_ctx *foot, struct geom_ctx *g)
{
        struct tworow_ctx *ctx = (struct tworow_ctx *) foot;
        double silkmaxy_pad;
        double silkmaxy_pack;
        double silkmaxy;
        double silkmaxx_pack;
        double silkmaxx_pad;
        double silkmaxx;
        double linew = ctx->foot.cfg.silk.linew;

        /* silk screen to pad clearence */
        silkmaxy_pad = extrpad(ctx)->y + 1.5 * linew;
        silkmaxy_pad = silkroundup(&ctx->foot, silkmaxy_pad);
        /*
         * [OPTIM] page 116: Pad to silkscreen rule overrides maximum component
         * body
         */
        silkmaxy_pack = ctx->foot.body.l.max / 2;
        silkmaxy_pack = silksnap(&ctx->foot, silkmaxy_pack);
        silkmaxy = fmax(
                silkmaxy_pad,
                silkmaxy_pack
        );

        /* NOTE: all silk x is insdie silkmaxx  */
        silkmaxx_pad = extrpad(ctx)->x;
        silkmaxx_pad = silkroundup(&ctx->foot, silkmaxx_pad);
        silkmaxx_pack = ctx->foot.body.w.max / 2;
        silkmaxx_pack = silksnap(&ctx->foot, silkmaxx_pack);
        if (silkmaxx_pad <= silkmaxx_pack) {
                err(
                        ctx,
                        "silkmaxx_pad=%f <= silkmaxx_pack=%f\n",
                        silkmaxx_pad,
                        silkmaxx_pack
                );
                return FOOT_FAIL;
        }
        /* if not true then we need to mark pin 1 some other way... */

        for (int i = 0; i < 2; i++) {
                double sgn = (double []) {-1, 1}[i];
                geom_moveto(g, + silkmaxx_pack, sgn * silkmaxy_pad);
                geom_lineto(g, + silkmaxx_pack, sgn * silkmaxy);
                geom_lineto(g, - silkmaxx_pack, sgn * silkmaxy);
                geom_lineto(g, - silkmaxx_pack, sgn * silkmaxy_pad);
        }
        silkmaxx = silkmaxx_pack;
        if (2 < ctx->comp.nleads) {
                geom_lineto(g, - silkmaxx_pad, silkmaxy_pad);
                silkmaxx = silkmaxx_pad;
        } else if (ctx->comp.pol) {
                /* place a polarity dot */
                double ymid = (silkmaxy_pad + silkmaxy_pack) / 2;
                silkmaxx = ctx->foot.body.w.max / 2 + 3 * linew;
                silkmaxx = silkroundup(&ctx->foot, silkmaxx);
                geom_circle(g, -silkmaxx, ymid, linew / 2);
        } else {
                ;
        }
        extrsilk(ctx)->y = silkmaxy + 0.5 * linew;
        extrsilk(ctx)->x = silkmaxx + 0.5 * linew;

        return FOOT_OK;
}

int foot_tworow_drawassy_golden_or_rect(
        struct foot_ctx *foot,
        struct geom_ctx *g
)
{
        struct tworow_ctx *ctx = (struct tworow_ctx *) foot;
        if (ctx->comp.pol) {
                return foot_drawgoldenassy(&ctx->foot, g);
        }
        return foot_drawrectassy(&ctx->foot, g);
}

int foot_tworow_dotagitem(struct tworow_ctx *ctx, const struct footag_item *ti)
{
        const union footag_data *td = &ti->data;
        switch (ti->tag) {
        case FOOTAG_LEADS:              ctx->comp.nleads        = td->i; break;
        case FOOTAG_PITCH:              ctx->comp.pitch         = td->d; break;
        default: break;
        }
        return FOOT_OK;
}

