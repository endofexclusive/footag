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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <footag/fout_cairo.h>
#include <cairo-svg.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include "fout_priv.h"

#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif

static const struct fout_ops THEOPS;

struct fout_cairo_ctx {
        struct fout_ctx fout;
        cairo_t *cr;
        cairo_surface_t *surface;
};

struct rgbaspec { double r, g, b, a; };

#define VALTOCOL(h) (h / 255.0)
#define VALTORGBA(_r, _g, _b, _a) { \
        .r = VALTOCOL(_r), \
        .g = VALTOCOL(_g), \
        .b = VALTOCOL(_b), \
        .a = (1.0 * _a), \
}

/*
 * Solarized colors
 *
 * a: alpha value
 */
#define sol_base03(a)   VALTORGBA(  0,  43,  54, a)
#define sol_base02(a)   VALTORGBA(  7,  54,  66, a)
#define sol_base01(a)   VALTORGBA( 88, 110, 117, a)
#define sol_base00(a)   VALTORGBA(101, 123, 131, a)
#define sol_base0(a)    VALTORGBA(131, 148, 150, a)
#define sol_base1(a)    VALTORGBA(147, 161, 161, a)
#define sol_base2(a)    VALTORGBA(238, 232, 213, a)
#define sol_base3(a)    VALTORGBA(253, 246, 227, a)
#define sol_yellow(a)   VALTORGBA(181, 137,   0, a)
#define sol_orange(a)   VALTORGBA(203,  75,  22, a)
#define sol_red(a)      VALTORGBA(220,  50,  47, a)
#define sol_magenta(a)  VALTORGBA(211,  54, 130, a)
#define sol_violet(a)   VALTORGBA(108, 113, 196, a)
#define sol_blue(a)     VALTORGBA( 38, 139, 210, a)
#define sol_cyan(a)     VALTORGBA( 42, 161, 152, a)
#define sol_green(a)    VALTORGBA(133, 153,   0, a)

static const struct rgbaspec UNKNOWNRGBA = { 0.0, 1.0, 0.0, 0.9 };
static const struct rgbaspec BACKRGBA = sol_base03(1);
static const struct rgbaspec THERGBA[GEOM_LAYER_NUM] = {
        [GEOM_LAYER_COPPER]     = sol_orange(1),
        [GEOM_LAYER_SILK]       = sol_base1(1),
        [GEOM_LAYER_ASSY]       = sol_base0(0.5),
        [GEOM_LAYER_CYARD]      = sol_blue(1),
};

static void setrgba(cairo_t *cr, const struct rgbaspec *c)
{
        cairo_set_source_rgba(cr, c->r, c->g, c->b, c->a);
}

static int prelayer(struct fout_ctx *fout, int layer)
{
        struct fout_cairo_ctx *ctx = (struct fout_cairo_ctx *) fout;
        const struct rgbaspec *c;

        c = &THERGBA[layer];
        if (layer < 0 || GEOM_LAYER_NUM <= layer) {
                c = &UNKNOWNRGBA;
        }
        setrgba(ctx->cr, c);

        return 0;
}

static int rectpad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        cairo_rectangle(
                ctx->cr,
                pad->x - pad->w / 2,
                pad->y - pad->h / 2,
                pad->w,
                pad->h
        );
        cairo_fill(ctx->cr);
        return FOUT_OK;
}

/* "rounded rectangle" */
static int rrectpad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        cairo_t *cr = ctx->cr;
        cairo_new_sub_path(cr);
        double w = pad->w;
        double h = pad->h;
        double x = pad->x - pad->w / 2;
        double y = pad->y - pad->h / 2;
        double r = pad->param;
        double q = M_PI / 2;

        cairo_arc(cr,   x + w - r,    y + h - r,      r,      0 * q,  1 * q);
        cairo_arc(cr,   x     + r,    y + h - r,      r,      1 * q,  2 * q);
        cairo_arc(cr,   x     + r,    y     + r,      r,      2 * q,  3 * q);
        cairo_arc(cr,   x + w - r,    y     + r,      r,      3 * q,  4 * q);

        cairo_close_path(cr);
        cairo_fill(cr);
        return FOUT_OK;
}

static void disc(struct fout_cairo_ctx *ctx, double x, double y, double r)
{
        cairo_new_sub_path(ctx->cr);
        cairo_arc(ctx->cr, x, y, r, 0, 2 * M_PI);
        cairo_close_path(ctx->cr);
        cairo_fill(ctx->cr);
}

static int circpad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        disc(ctx, pad->x, pad->y, pad->w / 2);
        return FOUT_OK;
}

static int tcircpad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        circpad(ctx, pad);
        setrgba(ctx->cr, &BACKRGBA);
        disc(ctx, pad->x, pad->y, pad->holew / 2);
        return FOUT_OK;
}

static int trectpad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        rectpad(ctx, pad);
        setrgba(ctx->cr, &BACKRGBA);
        disc(ctx, pad->x, pad->y, pad->holew / 2);
        return FOUT_OK;
}

static int dopad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad);

/* drawing shapes with cairo is very convenient */
static int doshape(struct fout_ctx *fout, const struct geom_shape *s)
{
        struct fout_cairo_ctx *ctx = (struct fout_cairo_ctx *) fout;
        int ret = FOUT_OK;

        cairo_set_line_width(ctx->cr, s->linew);
        if (s->type == GEOM_SHAPE_RECT) {
                const struct geom_rect *r = &s->shape.rect;
                cairo_rectangle(ctx->cr, r->x, r->y, r->w, r->h);
                cairo_stroke(ctx->cr);
        } else if (s->type == GEOM_SHAPE_LINE) {
                const struct geom_line *l = &s->shape.line;
                cairo_move_to(ctx->cr, l->x0, l->y0);
                cairo_line_to(ctx->cr, l->x1, l->y1);
                cairo_stroke(ctx->cr);
        } else if (s->type == GEOM_SHAPE_CIRCLE) {
                const struct geom_circle *c = &s->shape.circle;
                cairo_arc(ctx->cr, c->x, c->y, c->r, 0.0, 2 * M_PI);
                cairo_stroke(ctx->cr);
        } else if (s->type == GEOM_SHAPE_PAD) {
                ret = dopad(ctx, &s->shape.pad);
        } else {
                ret = FOUT_FAIL;
        }

        return ret;
}

static int (*const THEPADFUNCS[GEOM_PAD_NUM])(
        struct fout_cairo_ctx *ctx,
        const struct geom_pad *pad
) = {
        [GEOM_PAD_RECT]         = rectpad,
        [GEOM_PAD_RRECT]        = rrectpad,
        [GEOM_PAD_CIRC]         = circpad,
        [GEOM_PAD_TRECT]        = trectpad,
        [GEOM_PAD_TCIRC]        = tcircpad,
};

static int dopad(struct fout_cairo_ctx *ctx, const struct geom_pad *pad)
{
        int ret = FOUT_FAIL;
        int type = pad->type;

        if (type < 0 || GEOM_PAD_NUM <= type) {
                return FOUT_FAIL;
        }

        /* default pad color because the func() can change it */
        setrgba(ctx->cr, &THERGBA[GEOM_LAYER_COPPER]);
        ret = THEPADFUNCS[pad->type](ctx, pad);

        return ret;
}

static int fini(struct fout_ctx *fout)
{
        struct fout_cairo_ctx *ctx = (struct fout_cairo_ctx *) fout;
        if (!ctx) {
                return FOUT_OK;
        }

        if (ctx->cr) {
                cairo_destroy(ctx->cr);
        }
        if (ctx->surface) {
                cairo_surface_destroy(ctx->surface);
        }
        free(ctx);

        return FOUT_OK;
}

static double mmtodev(double mm)
{
        return mm * 72.0 / 25.4;
}

static int dothecairo(
        struct fout_cairo_ctx *ctx,
        double width,
        double height,
        double scale
)
{
        cairo_status_t status;

        ctx->cr = cairo_create(ctx->surface);
        status = cairo_status(ctx->cr);
        if (status != CAIRO_STATUS_SUCCESS) {
                fprintf(
                        stderr,
                        "fout_cairo: %s\n",
                        cairo_status_to_string(status)
                );
                return 1;
        }

        cairo_t *cr = ctx->cr;
        /*
         * You can think of cairo_scale() as multiplying all the following
         * coordinates with its argument.
         */
        cairo_scale(cr, scale * mmtodev(1), -scale * mmtodev(1));
        cairo_translate(cr, width / 2, -height / 2);
        setrgba(ctx->cr, &BACKRGBA);
        cairo_paint(cr);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

        return 0;
}

static struct fout_cairo_ctx *init0(void)
{
        struct fout_cairo_ctx *ctx;

        ctx = calloc(1, sizeof *ctx);
        if (ctx) {
                ctx->fout.ops = &THEOPS;
        }
        return ctx;
}

enum {
        FOUT_CAIRO_SVG,
        FOUT_CAIRO_PDF,
        FOUT_CAIRO_PS,
};

static struct fout_ctx *open_what(
        const char *filename,
        double width,
        double height,
        double scale,
        int what
)
{
        int ret;
        struct fout_cairo_ctx *ctx;
        cairo_status_t status;

        ctx = init0();
        if (!ctx) {
                goto out;
        }

        if (FOUT_CAIRO_SVG == what) {
                ctx->surface = cairo_svg_surface_create(
                        filename,
                        scale * mmtodev(width),
                        scale * mmtodev(height)
                );
        } else if (FOUT_CAIRO_PDF == what) {
                ctx->surface = cairo_pdf_surface_create(
                        filename,
                        scale * mmtodev(width),
                        scale * mmtodev(height)
                );
        } else if (FOUT_CAIRO_PS == what) {
                ctx->surface = cairo_ps_surface_create(
                        filename,
                        scale * mmtodev(width),
                        scale * mmtodev(height)
                );
        } else {
                goto out;
        }

        status = cairo_surface_status(ctx->surface);
        if (status != CAIRO_STATUS_SUCCESS) {
                fprintf(
                        stderr,
                        "fout_cairo: %s: %s\n",
                        filename,
                        cairo_status_to_string(status)
                );
                goto out;
        }

        ret = dothecairo(ctx, width, height, scale);
        if (ret) {
                goto out;
        }

        return &ctx->fout;

out:
        fini(&ctx->fout);
        return NULL;
}

struct fout_ctx *fout_cairo_svg_open(
        const char *filename,
        double width,
        double height,
        double scale
)
{
        return open_what(filename, width, height, scale, FOUT_CAIRO_SVG);
}

/* NOTE: you can do cairo_show_page(ctx->cr); on PDF to begin new page. */
struct fout_ctx *fout_cairo_pdf_open(
        const char *filename,
        double width,
        double height,
        double scale
)
{
        return open_what(filename, width, height, scale, FOUT_CAIRO_PDF);
}

struct fout_ctx *fout_cairo_ps_open(
        const char *filename,
        double width,
        double height,
        double scale
)
{
        return open_what(filename, width, height, scale, FOUT_CAIRO_PS);
}

static const struct fout_ops THEOPS = {
        .desc           = "cairo",
        .fini           = fini,
        .setname        = fout_setname_none,
        .pregen         = fout_gen_none,
        .postgen        = fout_gen_none,
        .prelayer       = prelayer,
        .postlayer      = fout_layer_none,
        .doshape        = doshape,
};

