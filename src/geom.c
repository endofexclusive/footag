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

#include <stdlib.h>
#include <string.h>
#include <footag/geom.h>

char *strdup(const char *s);

struct geom_shape;

struct geom_ctx {
        struct geom_shape *shapelist;
        double x;
        double y;
        double linew;
        int layer;
        struct geom_padspec padspec;
};

/* Allocate space for a new shape node, copy from caller and add to list. */
static int add(struct geom_ctx *ctx, const struct geom_shape *shape)
{
        struct geom_shape *sh;

        sh = calloc(1, sizeof *sh);
        if (!sh) {
                return 1;
        }
        *sh = *shape;
        sh->layer = ctx->layer;
        sh->linew = ctx->linew;
        sh->next = ctx->shapelist;
        ctx->shapelist = sh;

        return 0;
}

/* remove a shape from list and return to caller */
static struct geom_shape *remove(struct geom_ctx *ctx)
{
        struct geom_shape *sh = ctx->shapelist;
        if (!sh) {
                return NULL;
        }
        ctx->shapelist = sh->next;
        sh->next = NULL;

        return sh;
}

static const struct geom_padspec DEFAULT_PADSPEC = {
        .type   = 0,
        .w      = 1.0,
        .h      = 1.0,
        .holew  = 0.5,
        .holeh  = 0.5,
        .param  = 0.2,
};

struct geom_ctx *geom_open(void)
{
        struct geom_ctx *ctx;

        ctx = calloc(1, sizeof *ctx);
        if (!ctx) {
                goto out;
        }
        ctx->shapelist  = NULL;
        ctx->x          = 0.0;
        ctx->y          = 0.0;
        ctx->linew      = 0.1;
        ctx->layer      = 0;
        geom_setpadspec(ctx, &DEFAULT_PADSPEC);

        return ctx;

out:
        geom_close(ctx);
        return NULL;
}

static void freeshape(struct geom_shape *sh)
{
        if (GEOM_SHAPE_PAD == sh->type) {
                free(sh->shape.pad.name);
        }
        free(sh);
}

int geom_close(struct geom_ctx *ctx)
{
        if (!ctx) {
                return 0;
        }

        /* free anytning on ctx->shapelist */
        {
                struct geom_shape *sh;
                while (NULL != (sh = remove(ctx))) {
                        freeshape(sh);
                }
        }
        free(ctx);

        return 0;
}

int geom_getshapelist(
        const struct geom_ctx *ctx,
        const struct geom_shape **list
)
{
        *list = ctx->shapelist;
        return 0;
}

/* Set layer for draw operations */
int geom_setlayer(struct geom_ctx *ctx, int layer)
{
        ctx->layer = layer;
        return 0;
}

/* Set line width for draw operations */
int geom_setlinew(struct geom_ctx *ctx, double linew)
{
        ctx->linew = linew;
        return 0;
}

int geom_moveto(struct geom_ctx *ctx, double x, double y)
{
        ctx->x = x;
        ctx->y = y;
        return 0;
}

/* Place line with endpoints (x0,y0) and (x1,y1). */
int geom_line(struct geom_ctx *ctx, double x0, double y0, double x1, double y1)
{
        if (x0 == x1 && y0 == y1) {
                return 0;
        }
        const struct geom_shape shape = {
                .type = GEOM_SHAPE_LINE,
                .shape.line = {
                        .x0 = x0,
                        .y0 = y0,
                        .x1 = x1,
                        .y1 = y1,
                },
        };
        return add(ctx, &shape);
}

/* Place line from current pos to (x,y) and move current pos. */
int geom_lineto(struct geom_ctx *ctx, double x, double y)
{
        int ret;
        ret = geom_line(ctx, ctx->x, ctx->y, x, y);
        if (ret) { return ret; }
        return geom_moveto(ctx, x, y);
}

/* Place line centered at current pos. */
int geom_cline(struct geom_ctx *ctx, double dx, double dy)
{
        return geom_line(
                ctx,
                ctx->x - dx / 2,
                ctx->y - dy / 2,
                ctx->x + dx / 2,
                ctx->y + dy / 2
        );
}

/* Place rectangle. */
int geom_rect(struct geom_ctx *ctx, double x, double y, double w, double h)
{
        const struct geom_shape shape = {
                .type = GEOM_SHAPE_RECT,
                .shape.rect = {
                        .x = x,
                        .y = y,
                        .w = w,
                        .h = h,
                },
        };
        return add(ctx, &shape);
}

/* Place rectangle centered at current position. */
int geom_crect(struct geom_ctx *ctx, double w, double h)
{
        return geom_rect(
                ctx,
                ctx->x - w / 2,
                ctx->y - h / 2,
                w,
                h
        );
}

/* Place circle at (x,y) with radius r. */
int geom_circle(struct geom_ctx *ctx, double x, double y, double r)
{
        if (r == 0) {
                return 0;
        }
        const struct geom_shape shape = {
                .type = GEOM_SHAPE_CIRCLE,
                .shape.circle = {
                        .x = x,
                        .y = y,
                        .r = r,
                },
        };
        return add(ctx, &shape);
}

int geom_setpadspec(
        struct geom_ctx *ctx,
        const struct geom_padspec *spec
)
{
        ctx->padspec = *spec;
        return 0;
}

/* Place pad. */
int geom_pad(
        struct geom_ctx *ctx,
        const char *name,
        double x,
        double y
)
{
        int ret;

        const struct geom_shape shape = {
                .type = GEOM_SHAPE_PAD,
                .shape.pad = {
                        .name = strdup(name),
                        .type = ctx->padspec.type,
                        .x = x,
                        .y = y,
                        .w = ctx->padspec.w,
                        .h = ctx->padspec.h,
                        .holew = ctx->padspec.holew,
                        .holeh = ctx->padspec.holeh,
                        .param = ctx->padspec.param,
                },
        };
        if (!shape.shape.pad.name) {
                goto out;
        }
        ret = add(ctx, &shape);
        if (ret) {
                goto out;
        }

        return 0;

out:
        free(shape.shape.pad.name);

        return 1;
}

/* Place pad centered at current position. */
int geom_padc(struct geom_ctx *ctx, const char *name)
{
        return geom_pad(
                ctx,
                name,
                ctx->x,
                ctx->y
        );
}

