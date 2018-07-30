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

#ifndef FOOTAG_GEOM_H
#define FOOTAG_GEOM_H

/*
 * Immediate footprint representation
 */

enum {
        GEOM_OK,
        GEOM_FAIL
};

struct geom_ctx;

enum geom_shapetype {
        GEOM_SHAPE_LINE,
        GEOM_SHAPE_RECT,
        GEOM_SHAPE_CIRCLE,
        GEOM_SHAPE_PAD,
        GEOM_SHAPE_NUM,
};

/* template for placing pads */
struct geom_padspec {
        int type;
        double w, h;
        double holew, holeh;
        double param;
};

struct geom_line {
        double x0, y0;
        double x1, y1;
};

struct geom_rect {
        double x, y;
        double w, h;
};

struct geom_circle {
        double x, y;
        double r;
};

struct geom_pad {
        char *name;
        int type;
        double x, y;
        double w, h;
        double holew, holeh;
        double param;
};

struct geom_shape {
        struct geom_shape *next;
        enum geom_shapetype type;
        int layer;
        double linew;
        union {
                struct geom_line line;
                struct geom_rect rect;
                struct geom_circle circle;
                struct geom_pad pad;
        } shape;
};

struct geom_ctx *geom_open(void);

int geom_close(
        struct geom_ctx *ctx
);

int geom_getshapelist(
        const struct geom_ctx *ctx,
        const struct geom_shape **list
);

/* Set layer for draw operations */
int geom_setlayer(
        struct geom_ctx *ctx,
        int layer
);

/* Set line width for draw operations */
int geom_setlinew(
        struct geom_ctx *ctx,
        double linew
);

int geom_moveto(
        struct geom_ctx *ctx,
        double x,
        double y
);

/* Place line with endpoints (x0,y0) and (x1,y1). */
int geom_line(
        struct geom_ctx *ctx,
        double x0,
        double y0,
        double x1,
        double y1
);

/* Place line from current pos to (x,y) and move current pos. */
int geom_lineto(
        struct geom_ctx *ctx,
        double x,
        double y
);

/* Place line centered at current pos. */
int geom_cline(
        struct geom_ctx *ctx,
        double dx,
        double dy
);

/* Place rectangle. */
int geom_rect(
        struct geom_ctx *ctx,
        double x,
        double y,
        double w,
        double h
);

/* Place rectangle centered at current position. */
int geom_crect(
        struct geom_ctx *ctx,
        double w,
        double h
);

/* Place circle at (x,y) with radius r. */
int geom_circle(
        struct geom_ctx *ctx,
        double x,
        double y,
        double r
);

/*
 * Set pad template.
 *
 * NOTE: Pad parameters are never interpreted by this module
 */
int geom_setpadspec(
        struct geom_ctx *ctx,
        const struct geom_padspec *spec
);

/* Place pad centered at (x,y). */
int geom_pad(
        struct geom_ctx *ctx,
        const char *name,
        double x,
        double y
);

/* Place pad centered at current position. */
int geom_padc(
        struct geom_ctx *ctx,
        const char *name
);

#endif

