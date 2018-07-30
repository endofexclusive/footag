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
#include <footag/fout_trace.h>
#include "fout_priv.h"

#define FSTR "%-10s"

static const struct fout_ops THEOPS;

struct fout_trace_ctx {
        struct fout_ctx fout;
        FILE *stream;
};

static const char *LAYERNAME[GEOM_LAYER_NUM] = {
        [GEOM_LAYER_COPPER]     = "copper",
        [GEOM_LAYER_SILK]       = "silk",
        [GEOM_LAYER_ASSY]       = "assy",
        [GEOM_LAYER_CYARD]      = "cyard",
};

static const char *SHAPENAME[GEOM_SHAPE_NUM] = {
        [GEOM_SHAPE_LINE]       = "line",
        [GEOM_SHAPE_RECT]       = "rect",
        [GEOM_SHAPE_CIRCLE]     = "circle",
        [GEOM_SHAPE_PAD]        = "pad",
};

static const char *PADNAME[GEOM_PAD_NUM] = {
        [GEOM_PAD_RECT]         = "rect",
        [GEOM_PAD_RRECT]        = "rrect",
        [GEOM_PAD_CIRC]         = "circ",
        [GEOM_PAD_TRECT]        = "trect",
        [GEOM_PAD_TCIRC]        = "tcirc",
};

static int fini(struct fout_ctx *fout)
{
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        if (!ctx) {
                return FOUT_OK;
        }

        if (ctx->stream) {
                fprintf(ctx->stream, FSTR "\n", __func__);
                fclose(ctx->stream);
        }
        free(ctx);

        return FOUT_OK;
}

struct fout_ctx *fout_trace_open(const char *filename)
{
        struct fout_trace_ctx *ctx;

        ctx = calloc(1, sizeof *ctx);
        if (!ctx) {
                goto out;
        }
        ctx->fout.ops = &THEOPS;

        ctx->stream = fopen(filename, "w");
        if (!ctx->stream) {
                fprintf(
                        stderr,
                        "fout_trace: %s: %s\n",
                        filename,
                        strerror(errno)
                );
                goto out;
        }

        fprintf(ctx->stream, FSTR "%s\n", "open", filename);
        return &ctx->fout;

out:
        fini(&ctx->fout);
        return NULL;
}

static int dopad(struct fout_trace_ctx *ctx, const struct geom_pad *p)
{
        FILE *s = ctx->stream;
        fprintf(s, "name: %-4s", p->name);
        fprintf(s, "  type: %-5s", PADNAME[p->type]);
        fprintf(s, "  x: %7.3f,  y: %7.3f, ", p->x, p->y);
        if (p->type == GEOM_PAD_RECT) {
                fprintf(s, "  w: %7.3f, h: %7.3f", p->w, p->h);
        } else if (p->type == GEOM_PAD_RRECT) {
                fprintf(s, "  w: %7.3f, h: %7.3f", p->w, p->h);
                fprintf(s, ",  r: %7.3f", p->param);
        } else if (p->type == GEOM_PAD_CIRC) {
                fprintf(s, "  r: %7.3f", p->param);
        } else if (p->type == GEOM_PAD_TRECT) {
                fprintf(s, "  w: %7.3f, h: %7.3f", p->w, p->h);
                fprintf(s, "  hw: %7.3f, hh: %7.3f", p->holew, p->holeh);
        } else if (p->type == GEOM_PAD_TCIRC) {
                fprintf(s, "  w: %7.3f, h: %7.3f", p->w, p->h);
                fprintf(s, "  hw: %7.3f, hh: %7.3f", p->holew, p->holeh);
        } else {
                return FOUT_FAIL;
        }
        return FOUT_OK;
}

static int doshape(struct fout_ctx *fout, const struct geom_shape *s)
{
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        int ret = FOUT_OK;

        fprintf(ctx->stream, FSTR "", __func__);
        fprintf(ctx->stream, "%-7s", SHAPENAME[s->type]);
        if (s->type == GEOM_SHAPE_PAD) {
                ret = dopad(ctx, &s->shape.pad);
                fprintf(ctx->stream, "\n");
                return ret;
        }
        fprintf(ctx->stream, "linew: %4.3f", s->linew);
        if (s->type == GEOM_SHAPE_LINE) {
                const struct geom_line *l = &s->shape.line;
                fprintf(ctx->stream, ", x0:%8.3f, y0:%8.3f", l->x0, l->y0);
                fprintf(ctx->stream, ", x1:%8.3f, y1:%8.3f", l->x1, l->y1);
        } else if (s->type == GEOM_SHAPE_RECT) {
                const struct geom_rect *r = &s->shape.rect;
                fprintf(ctx->stream, ", x: %8.3f, y: %8.3f", r->x, r->y);
                fprintf(ctx->stream, ", w: %8.3f, h: %8.3f", r->w, r->h);
        } else if (s->type == GEOM_SHAPE_CIRCLE) {
                const struct geom_circle *c = &s->shape.circle;
                fprintf(ctx->stream, ", x: %8.3f, y: %8.3f", c->x, c->y);
                fprintf(ctx->stream, ", r: %8.3f", c->r);
        } else {
                ret = FOUT_FAIL;
        }
        fprintf(ctx->stream, "\n");

        return ret;
}

static int setname(struct fout_ctx *fout, const char *name)
{
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        fprintf(ctx->stream, FSTR "%s\n", __func__, name);
        return FOUT_OK;
}

static int pregen(struct fout_ctx *fout, const struct geom_shape *head)
{
        UNUSED(head);
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        fprintf(ctx->stream, FSTR "\n", __func__);
        return FOUT_OK;
}

static int postgen(struct fout_ctx *fout, const struct geom_shape *head)
{
        UNUSED(head);
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        fprintf(ctx->stream, FSTR "\n", __func__);
        return FOUT_OK;
}

static int prelayer(struct fout_ctx *fout, int layer)
{
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        fprintf(ctx->stream, FSTR "%s\n", __func__, LAYERNAME[layer]);
        return FOUT_OK;
}


static int postlayer(struct fout_ctx *fout, int layer)
{
        struct fout_trace_ctx *ctx = (struct fout_trace_ctx *) fout;
        fprintf(ctx->stream, FSTR "%s\n", __func__, LAYERNAME[layer]);
        return FOUT_OK;
}

static const struct fout_ops THEOPS = {
        .desc           = "trace",
        .fini           = fini,
        .setname        = setname,
        .pregen         = pregen,
        .postgen        = postgen,
        .prelayer       = prelayer,
        .postlayer      = postlayer,
        .doshape        = doshape,
};

