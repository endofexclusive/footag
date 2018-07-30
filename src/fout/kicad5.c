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

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <footag/fout_kicad5.h>
#include "fout_priv.h"

static const struct fout_ops THEOPS;

struct fout_kicad5_ctx {
        struct fout_ctx fout;
        FILE *stream;
        const char *name;
        /* used to estimate max text size on fabrication layer */
        double fabymin;
        double fabymax;
};

static const char *LAYERNAME[GEOM_LAYER_NUM] = {
        [GEOM_LAYER_COPPER]     = "F.Cu F.Paste F.Mask",
        [GEOM_LAYER_SILK]       = "F.SilkS",
        [GEOM_LAYER_ASSY]       = "F.Fab",
        [GEOM_LAYER_CYARD]      = "F.CrtYd",
};
/* NOTE: through-hole pad layer names are differ from smd */
static const char *LAYERNAME_TPAD = "*.Cu *.Mask";

static int fini(struct fout_ctx *fout)
{
        struct fout_kicad5_ctx *ctx = (struct fout_kicad5_ctx *) fout;
        if (!ctx) {
                return FOUT_OK;
        }

        if (ctx->stream) {
                fclose(ctx->stream);
        }
        free(ctx);

        return FOUT_OK;
}

struct fout_ctx *fout_kicad5_open(const char *filename)
{
        struct fout_kicad5_ctx *ctx;

        ctx = calloc(1, sizeof *ctx);
        if (!ctx) {
                goto out;
        }
        ctx->fout.ops = &THEOPS;

        ctx->stream = fopen(filename, "w");
        if (!ctx->stream) {
                fprintf(
                        stderr,
                        "fout_kicad5: %s: %s\n",
                        filename,
                        strerror(errno)
                );
                goto out;
        }

        ctx->fabymin = -0.40;
        ctx->fabymax =  0.40;
        return &ctx->fout;

out:
        fini(&ctx->fout);
        return NULL;
}

static int rectpad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        FILE *s = ctx->stream;
        fprintf(s, "  (pad \"%s\" smd rect", pad->name);
        fprintf(s, " (at %f %f)", pad->x, -pad->y);
        fprintf(s, " (size %f %f)", pad->w, pad->h);
        fprintf(s, " (layers %s)", LAYERNAME[GEOM_LAYER_COPPER]);
        fprintf(s, ")\n");
        return FOUT_OK;
}

/* "rounded rectangle" */
static int rrectpad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        FILE *s = ctx->stream;
        double ratio;
        ratio = pad->param / fmin(pad->w, pad->h);
        fprintf(s, "  (pad \"%s\" smd roundrect", pad->name);
        fprintf(s, " (at %f %f)", pad->x, -pad->y);
        fprintf(s, " (size %f %f)", pad->w, pad->h);
        fprintf(s, " (layers %s)", LAYERNAME[GEOM_LAYER_COPPER]);
        fprintf(s, " (roundrect_rratio %f)", ratio);
        fprintf(s, ")\n");
        return FOUT_OK;
}

static int circpad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        FILE *s = ctx->stream;
        fprintf(s, "  (pad \"%s\" smd circle", pad->name);
        fprintf(s, " (at %f %f)", pad->x, -pad->y);
        fprintf(s, " (size %f %f)", pad->w, pad->h);
        fprintf(s, " (layers %s)", LAYERNAME[GEOM_LAYER_COPPER]);
        fprintf(s, ")\n");
        return FOUT_OK;
}

static int tcircpad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        FILE *s = ctx->stream;
        fprintf(s, "  (pad \"%s\" thru_hole circle", pad->name);
        fprintf(s, " (at %f %f)", pad->x, -pad->y);
        fprintf(s, " (size %f %f)", pad->w, pad->h);
        fprintf(s, " (drill %f)", pad->holew);
        fprintf(s, " (layers %s)", LAYERNAME_TPAD);
        fprintf(s, ")\n");
        return FOUT_OK;
}

static int trectpad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        FILE *s = ctx->stream;
        fprintf(s, "  (pad \"%s\" thru_hole rect", pad->name);
        fprintf(s, " (at %f %f)", pad->x, -pad->y);
        fprintf(s, " (size %f %f)", pad->w, pad->h);
        fprintf(s, " (drill %f)", pad->holew);
        fprintf(s, " (layers %s)", LAYERNAME_TPAD);
        fprintf(s, ")\n");
        return FOUT_OK;
}

static int (*const THEPADFUNCS[GEOM_PAD_NUM])(
        struct fout_kicad5_ctx *ctx,
        const struct geom_pad *pad
) = {
        [GEOM_PAD_RECT]         = rectpad,
        [GEOM_PAD_RRECT]        = rrectpad,
        [GEOM_PAD_CIRC]         = circpad,
        [GEOM_PAD_TRECT]        = trectpad,
        [GEOM_PAD_TCIRC]        = tcircpad,
};

static int dopad(struct fout_kicad5_ctx *ctx, const struct geom_pad *pad)
{
        int type = pad->type;
        if (type < 0 || GEOM_PAD_NUM <= type) {
                return FOUT_FAIL;
        }
        return THEPADFUNCS[pad->type](ctx, pad);
}

static int kiline(
        struct fout_kicad5_ctx *ctx,
        double x0, double y0,
        double x1, double y1,
        double linew,
        int layer
)
{
        FILE *s = ctx->stream;
        fprintf(s, "  (fp_line");
        fprintf(s, " (start %f %f)", x0, -y0);
        fprintf(s, " (end %f %f)", x1, -y1);
        fprintf(s, " (layer %s)", LAYERNAME[layer]);
        fprintf(s, " (width %f)", linew);
        fprintf(s, ")\n");
        if (layer == GEOM_LAYER_ASSY) {
                ctx->fabymin = fmin(ctx->fabymin, y0);
                ctx->fabymin = fmin(ctx->fabymin, y1);
                ctx->fabymax = fmax(ctx->fabymax, y0);
                ctx->fabymax = fmax(ctx->fabymax, y1);
        }
        return FOUT_OK;
}

static int doshape(struct fout_ctx *fout, const struct geom_shape *s)
{
        struct fout_kicad5_ctx *ctx = (struct fout_kicad5_ctx *) fout;
        int ret = FOUT_OK;
        int layer = s->layer;
        double linew = s->linew;

        if (s->type == GEOM_SHAPE_LINE) {
                const struct geom_line *l = &s->shape.line;
                kiline(ctx, l->x0, l->y0, l->x1, l->y1, linew, layer);
        } else if (s->type == GEOM_SHAPE_RECT) {
                const struct geom_rect *r = &s->shape.rect;
                double x = r->x, y = r->y, w = r->w, h = r->h;
                kiline(ctx, x,   y,   x+w, y,   linew, layer);
                kiline(ctx, x+w, y,   x+w, y+h, linew, layer);
                kiline(ctx, x+w, y+h, x,   y+h, linew, layer);
                kiline(ctx, x,   y+h, x,   y,   linew, layer);
        } else if (s->type == GEOM_SHAPE_CIRCLE) {
                const struct geom_circle *c = &s->shape.circle;
                FILE *f = ctx->stream;
                fprintf(f, "  (fp_circle");
                fprintf(f, " (center %f %f)", c->x, -c->y);
                fprintf(f, " (end %f %f)", c->x + c->r, -c->y);
                fprintf(f, " (layer %s)", LAYERNAME[layer]);
                fprintf(f, " (width %f)", linew);
                fprintf(f, ")\n");
        } else if (s->type == GEOM_SHAPE_PAD) {
                ret = dopad(ctx, &s->shape.pad);
                return ret;
        } else {
                ret = FOUT_FAIL;
        }

        return ret;
}

static int setname(struct fout_ctx *fout, const char *name)
{
        struct fout_kicad5_ctx *ctx = (struct fout_kicad5_ctx *) fout;
        ctx->name = name;
        return FOUT_OK;
}

static int pregen(struct fout_ctx *fout, const struct geom_shape *head)
{
        UNUSED(head);
        struct fout_kicad5_ctx *ctx = (struct fout_kicad5_ctx *) fout;
        fprintf(ctx->stream, "(module %s", ctx->name);
        /* NOTE: The purpose of this 'layer' is unknown */
        fprintf(ctx->stream, " (layer F.Cu)");
        fprintf(ctx->stream, " (tedit %lX )", (long) time(NULL));
        fprintf(ctx->stream, "\n");
        return FOUT_OK;
}

/*
 * - refdes on silk
 * - refdes on assy
 * - value  on eco1
 *
 * refdes location on silkscreen is a pcb policy thing. we put it
 * slightly below for convenience during initial placement
 */
static int postgen(struct fout_ctx *fout, const struct geom_shape *head)
{
        UNUSED(head);
        struct fout_kicad5_ctx *ctx = (struct fout_kicad5_ctx *) fout;
        FILE *s = ctx->stream;

        double h = 1.20;
        double w = 0.10 * h;
        double x = 0;
        double silkx = 0;
        double silky = ceil(ctx->fabymax) + 1.0;

        /* move fab refdes out of outline if it does not fit */
        if ((1.20 * (ctx->fabymax - ctx->fabymin)) < h) {
            x = 3 * h;
        }

        fprintf(s, "  (fp_text reference REF** (at %f %f)", silkx, silky);
        fprintf(s, " (layer F.SilkS)");
        fprintf(s, "\n");
        fprintf(s, "    (effects");
        fprintf(s, " (font");
        fprintf(s, " (size %f %f)", h, h);
        fprintf(s, " (thickness %f)", w);
        fprintf(s, ")");
        fprintf(s, ")\n");
        fprintf(s, "  )\n");

        fprintf(s, "  (fp_text user %%R (at %f 0)", x);
        fprintf(s, " (layer F.Fab)");
        fprintf(s, "\n");
        fprintf(s, "    (effects");
        fprintf(s, " (font");
        fprintf(s, " (size %f %f)", h, h);
        fprintf(s, " (thickness %f)", w);
        fprintf(s, ")");
        fprintf(s, ")\n");
        fprintf(s, "  )\n");

        fprintf(s, "  (fp_text value VAL** (at %f %f)", 0.0, 0.0);
        fprintf(s, " (layer Eco1.User)");
        fprintf(s, "\n");
        fprintf(s, "    (effects");
        fprintf(s, " (font");
        fprintf(s, " (size %f %f)", h, h);
        fprintf(s, " (thickness %f)", w);
        fprintf(s, ")");
        fprintf(s, ")\n");
        fprintf(s, "  )\n");

        fprintf(s, ")\n");
        fprintf(s, "\n");
        fprintf(s, "# generated by Footag \n");
        fprintf(s, "# file: %s\n", __FILE__);
        fprintf(s, "# date: %s\n", __DATE__);
        fprintf(s, "# time: %s\n", __TIME__);

        return FOUT_OK;
}

static const struct fout_ops THEOPS = {
        .desc           = "kicad5",
        .fini           = fini,
        .setname        = setname,
        .pregen         = pregen,
        .postgen        = postgen,
        .prelayer       = fout_layer_none,
        .postlayer      = fout_layer_none,
        .doshape        = doshape,
};

