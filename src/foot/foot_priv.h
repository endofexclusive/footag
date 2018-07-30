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

#ifndef FOOT_PRIV_H
#define FOOT_PRIV_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <footag/foot.h>
#include <footag/geom.h>
#include <footag/geom_types.h>
#define FOOLOG_SUBSYS "foot"
#include "foolog.h"

char *strdup(const char *s);

#ifndef UNUSED
 #define UNUSED(i) (void) (sizeof (i))
#endif

#ifndef NELEM
 #define NELEM(a) ((sizeof a) / (sizeof (a[0])))
#endif

/*
 *    |-------- span --------|
 * _   ________      ________
 *    |        |    |        |
 * w  |        |    |        |
 * _  |________|    |________|
 *
 *    | length |
 */
struct smdlead {
        struct footol span;   /* lead span, "l", "H" */
        struct footol length; /* "t", "L" */
        struct footol width;  /* "B" */
};

struct foot_layercfg {
        double linew;
        double grid;
};

/*
 * Footprint, project and library policy
 *
 * Can be set by padcalc() if not set earlier. and if padcalc()
 * does not set it, then calc() can do it. And user could change it
 * afterwards aswell.
 *
 * 1. user after open()
 * 2. padcalc()
 * 3. calc()
 * 4. user before gen()
 */
struct foot_cfg {
        struct foot_layercfg land;
        struct foot_layercfg silk;
        struct foot_layercfg assy;
        /* Also the IPC-7351B courtyard round-off (up?) factor */
        struct foot_layercfg cyard;
        double cyexc;   /* Courtyard excess */
        double f;       /* PCB fabrication tolerance */
        double p;       /* Part placement tolerance */
};

enum {
        PINNAME_BUFSIZE = 32,
};

typedef struct foot_ctx *(foot_initfunc)(
        struct foot_ctx *ctx,
        const struct footag_item *tags
);
typedef int (foot_finifunc)(
        struct foot_ctx *ctx
);
struct foot_ops {
        /* padspec init value. copied before init. user tags can override. */
        struct geom_padspec padspec;
        int (*calcpads)(struct foot_ctx *ctx);
        int (*drawpads)(struct foot_ctx *ctx, struct geom_ctx *g);
        int (*setpad)(struct foot_ctx *ctx, struct geom_ctx *g, int n);
        int (*drawsilk)(struct foot_ctx *ctx, struct geom_ctx *g);
        int (*drawassy)(struct foot_ctx *ctx, struct geom_ctx *g);
        int (*calccyard)(struct foot_ctx *ctx);
        int (*drawcyard)(struct foot_ctx *ctx, struct geom_ctx *g);
        char *(*name)(struct foot_ctx *ctx, const char *pre, const char *suf);
        foot_initfunc *init;
        foot_finifunc *fini;
        size_t size;
};

struct foot_coord { double x, y; };

/*
 * Interpratation of these fields may vary between component types. Most are
 * rectangular with some height and these are straight forward. More exotic
 * styles include aluminum electrolytic capacitors and through hole compnents
 * with vertical mounting.
 *
 * Nominal dimensions are:
 *
 *  |- w -|
 *   _____    _
 * -|     |-
 * -|     |-  l
 * -|_____|-  _
 */
struct foot_body {
        struct footol l; /* body length */
        struct footol w; /* body width */
        struct footol h; /* body height */
};

struct foot_ctx {
        struct foolog_ctx foolog;
        enum footype type;
        const struct foot_ops *ops;
        char **pinnames;
        struct foot_body body;
        /* measured from origin, assumed to mirror */
        struct {
                struct foot_coord pad;
                struct foot_coord silk;
                /* pacakge extremes are in comp */
        } extreme;
        struct foot_coord cyard;
        struct foot_cfg cfg;
        /* generated internally. user could update pad after calc()  */
        struct geom_padspec pad;
};

/* Return a deep copy of the stringlist name */
char **foot_stringlist_clone(
        const char *const *name
);
/* Free the stringlist. Does nothing if list is NULL */
int foot_stringlist_free(
        char **list
);
int foot_stringlist_nelem(
        const char *const *name
);
/* Locate the first occurrence of str */
char *const *foot_stringlist_find(
        char *const *list,
        const char *str
);

/* Returns number of pins marked as skip ("-") in pinnames. */
int foot_nskip(struct foot_ctx *ctx);

/* NOTE: snap is an operation and grid is a property */
static inline double snap(double v, double grid)
{
        if (!grid) { return v; }
        return round(v / grid) * grid;
}

static inline double landsnap(const struct foot_ctx *ctx, double v)
{
        return snap(v, ctx->cfg.land.grid);
}

static inline double silksnap(const struct foot_ctx *ctx, double v)
{
        return snap(v, ctx->cfg.silk.grid);
}

static inline double roundup(double v, double grid)
{
        if (!grid) { return v; }
        return ceil(v / grid) * grid;
}

static inline double silkroundup(const struct foot_ctx *ctx, double v)
{
        return roundup(v, ctx->cfg.silk.grid);
}

int foot_setpad_default(struct foot_ctx *ctx, struct geom_ctx *g, int n);
int foot_setpad_th(struct foot_ctx *ctx, struct geom_ctx *g, int n);
void foot_drawcorner(struct geom_ctx *g, double x, double y, double l);
int foot_drawsilk_corners(struct foot_ctx *ctx, struct geom_ctx *g);
int foot_drawgoldenassy(struct foot_ctx *ctx, struct geom_ctx *g);
int foot_drawrectassy(struct foot_ctx *ctx, struct geom_ctx *g);
int foot_drawcyard_ipcb(struct foot_ctx *ctx, struct geom_ctx *g);
int foot_calccyard_ipcb(struct foot_ctx *ctx);
int foot_smdlead_dotagitem(
        struct smdlead *lead,
        int tag,
        const union footag_data *td
);
char *foot_name_none(struct foot_ctx *ctx, const char *pre, const char *suf);
size_t foot_bodyprint_wlh(const struct foot_body *body, char *p, size_t len);
size_t foot_bodyprint_wl(const struct foot_body *body, char *p, size_t len);
int foot_fini_none(struct foot_ctx *foot);

extern const struct foot_ops FOOT_OPS_CHIP_IPCB;
extern const struct foot_ops FOOT_OPS_MOLDED_IPCB;
extern const struct foot_ops FOOT_OPS_SOIC_IPCB;
extern const struct foot_ops FOOT_OPS_SIDECONCAVE_IPCB;
extern const struct foot_ops FOOT_OPS_DIP_IPC;
extern const struct foot_ops FOOT_OPS_BGA_IPCB;
extern const struct foot_ops FOOT_OPS_PGA_IPC;

#endif

