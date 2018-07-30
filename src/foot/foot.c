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

#include "foot_priv.h"

#define dbg(ctx, ...) foolog_msg(FOOLOG_LEVEL_DEBUG, &ctx->foolog, __VA_ARGS__)

/* the user can update the pads */
int foot_calc(
        struct foot_ctx *ctx
)
{
        return ctx->ops->calcpads(ctx);
}

int foot_gen(
        struct foot_ctx *ctx,
        struct geom_ctx *geom
)
{
        int ret;

        /* To rule out any misunderstanding */
        if (!geom) { return FOOT_FAIL; }

        ret = geom_setlayer(geom, GEOM_LAYER_COPPER);
        if (ret) { return FOOT_FAIL; }
        ret = geom_setlinew(geom, 0.05);
        if (ret) { return FOOT_FAIL; }
        ret = ctx->ops->drawpads(ctx, geom);
        if (ret) { return ret; }

        ret = geom_setlayer(geom, GEOM_LAYER_SILK);
        if (ret) { return FOOT_FAIL; }
        ret = geom_setlinew(geom, ctx->cfg.silk.linew);
        if (ret) { return FOOT_FAIL; }
        ret = ctx->ops->drawsilk(ctx, geom);
        if (ret) { return ret; }

        ret = geom_setlayer(geom, GEOM_LAYER_ASSY);
        if (ret) { return FOOT_FAIL; }
        ret = geom_setlinew(geom, ctx->cfg.assy.linew);
        if (ret) { return FOOT_FAIL; }
        ret = ctx->ops->drawassy(ctx, geom);
        if (ret) { return ret; }

        ret = ctx->ops->calccyard(ctx);
        if (ret) { return ret; }
        ret = geom_setlayer(geom, GEOM_LAYER_CYARD);
        if (ret) { return FOOT_FAIL; }
        ret = geom_setlinew(geom, ctx->cfg.cyard.linew);
        if (ret) { return FOOT_FAIL; }
        ret = ctx->ops->drawcyard(ctx, geom);
        if (ret) { return ret; }

        return FOOT_OK;
}

/* We set only for the first pad. It will remain in geom for the others. */
int foot_setpad_default(struct foot_ctx *ctx, struct geom_ctx *g, int n)
{
        if (n) { return FOOT_OK; }
        geom_setpadspec(g, &ctx->pad);
        return FOOT_OK;
}

/* First pin has rectpad, others have circpad. */
int foot_setpad_th(struct foot_ctx *ctx, struct geom_ctx *g, int n)
{
        if (n == 0) {
                ctx->pad.type = GEOM_PAD_TRECT;
                geom_setpadspec(g, &ctx->pad);
        } else if (n == 1) {
                ctx->pad.type = GEOM_PAD_TCIRC;
                geom_setpadspec(g, &ctx->pad);
        }
        return FOOT_OK;
}

void foot_drawcorner(struct geom_ctx *g, double x, double y, double l)
{
        double sx = x < 0.0 ? -1: 1;
        double sy = y < 0.0 ? -1: 1;
        geom_moveto(g, x,               y - sy * l);
        geom_lineto(g, x,               y);
        geom_lineto(g, x -  sx * l,     y);
}

int foot_drawsilk_corners(struct foot_ctx *ctx, struct geom_ctx *g)
{
        double silkmaxy_pad;
        double silkmaxy_pack;
        double silkmaxy;
        double silkmaxx_pack;
        double silkmaxx_pad;
        double silkmaxx;

        /* silk screen to pad clearence */
        silkmaxy_pad = ctx->extreme.pad.y + 1.5 * ctx->cfg.silk.linew;
        silkmaxy_pad = silkroundup(ctx, silkmaxy_pad);
        silkmaxy_pack = ctx->body.l.max / 2;
        silkmaxy_pack = silksnap(ctx, silkmaxy_pack);
        silkmaxy = fmax(
                silkmaxy_pad,
                silkmaxy_pack
        );

        silkmaxx_pad = ctx->extreme.pad.x + 1.5 * ctx->cfg.silk.linew;
        silkmaxx_pad = silkroundup(ctx, silkmaxx_pad);
        silkmaxx_pack = ctx->body.w.max / 2;
        silkmaxx_pack = silksnap(ctx, silkmaxx_pack);
        silkmaxx = fmax(
                silkmaxx_pad,
                silkmaxx_pack
        );

        double l = fmin(silkmaxx, silkmaxy) / FOOT_GOLDEN_RATIO;
        for (int i = 0 ; i < 3; i++) {
                const int xys[][2] = { {-1, -1}, {1,1}, {1,-1}};
                foot_drawcorner(
                        g,
                        xys[i][0] * silkmaxx,
                        xys[i][1] * silkmaxy,
                        l / 3
                );
        }
        foot_drawcorner(g, -silkmaxx, silkmaxy, l);

        ctx->extreme.silk.y = silkmaxy + 0.5 * ctx->cfg.silk.linew;
        ctx->extreme.silk.x = silkmaxx + 0.5 * ctx->cfg.silk.linew;

        return FOOT_OK;
}

/* Draw a rectangle with cutout at golden ratio */
static int foot_drawgolden(struct geom_ctx *g, double w, double h, double grid)
{
        double x0 = snap(w / 2, grid);
        double y0 = snap(h / 2, grid);
        double shortest = fmin(w, h);
        double c = snap(shortest / 2 / FOOT_GOLDEN_RATIO, grid);

        geom_moveto(g, -x0 + c, +y0);
        geom_lineto(g, +x0,     +y0);
        geom_lineto(g, +x0,     -y0);
        geom_lineto(g, -x0,     -y0);
        geom_lineto(g, -x0,     +y0 - c);
        geom_lineto(g, -x0 + c, +y0);

        return FOOT_OK;
}

/* fancy component outline */
int foot_drawgoldenassy(struct foot_ctx *ctx, struct geom_ctx *g)
{
        return foot_drawgolden(
                g,
                ctx->body.w.max,
                ctx->body.l.max,
                ctx->cfg.assy.grid
        );
}

static int foot_drawrect(struct geom_ctx *g, double w, double h, double grid)
{
        double x0 = snap(w / 2, grid);
        double y0 = snap(h / 2, grid);
        geom_moveto(g, 0, 0);
        geom_crect(g, 2 * x0, 2 * y0);
        return FOOT_OK;
}

int foot_drawrectassy(struct foot_ctx *ctx, struct geom_ctx *g)
{
        return foot_drawrect(
                g,
                ctx->body.w.max,
                ctx->body.l.max,
                ctx->cfg.assy.grid
        );
}

static int foot_drawcrosshair(struct geom_ctx *g, double x, double y, double scale)
{
        geom_circle(g, x, y, scale * 0.25);
        geom_moveto(g, x, y);
        geom_cline(g, scale * 0.7, 0);
        geom_cline(g, 0, scale * 0.7);

        return FOOT_OK;
}

/* this is a IPC-7351B thing */
int foot_drawcyard_ipcb(struct foot_ctx *ctx, struct geom_ctx *g)
{
        foot_drawcrosshair(g, 0, 0, 1.0);
        geom_crect(g, 2 * ctx->cyard.x, 2 * ctx->cyard.y);

        return FOOT_OK;
}

int foot_calccyard_ipcb(struct foot_ctx *ctx)
{
        /* everything measure from origin */
        double maxx;
        double maxy;

        maxx = fmax(ctx->extreme.pad.x, ctx->extreme.silk.x);
        maxx = fmax(maxx, ctx->body.w.max / 2);
        maxy = fmax(ctx->extreme.pad.y, ctx->extreme.silk.y);
        maxy = fmax(maxy, ctx->body.l.max / 2);
        /* add courtyard excess */
        maxx += ctx->cfg.cyexc;
        maxy += ctx->cfg.cyexc;
        /* snap out with courtyard "round-up" */
        maxx = roundup(maxx, ctx->cfg.cyard.grid);
        maxy = roundup(maxy, ctx->cfg.cyard.grid);

        ctx->cyard.x = maxx;
        ctx->cyard.y = maxy;

        return FOOT_OK;
}

#define FOOTOL_TEMPLATE(n) { .nom = (n+0.0), .min = (n-0.1), .max = (n+0.1), }
static const struct foot_body FOOT_BODY_DEFAULT = {
        .l = FOOTOL_TEMPLATE(16.0),
        .w = FOOTOL_TEMPLATE(16.0),
        .h = FOOTOL_TEMPLATE(2.2),
};
#undef FOOTOL_TEMPLATE

static const struct foot_cfg FOOT_CFG_DEFAULT = {
        .land   = { .linew = 0.00, .grid = 0.05, },
        .silk   = { .linew = 0.12, .grid = 0.05, },
        .assy   = { .linew = 0.12, .grid = 0.05, },
        .cyard  = { .linew = 0.05, .grid = 0.00, },
        .cyexc  = 0.0,
        .f = 0.1,
        .p = 0.1,
};

/* happen to be a one-to-one mapping for now but that is not an invariant. */
static const int TO_GEOM_PAD[FOOTAG_PADTYPE_NUM] = {
        [FOOTAG_PADTYPE_RECT]   = GEOM_PAD_RECT,
        [FOOTAG_PADTYPE_RRECT]  = GEOM_PAD_RRECT,
        [FOOTAG_PADTYPE_CIRC]   = GEOM_PAD_CIRC,
        [FOOTAG_PADTYPE_TRECT]  = GEOM_PAD_TRECT,
        [FOOTAG_PADTYPE_TCIRC]  = GEOM_PAD_TCIRC,
};

static int dotagitem(
        struct foot_ctx *ctx,
        const struct footag_item *ti
)
{
        const union footag_data *td = &ti->data;
        switch (ti->tag) {
        case FOOTAG_BODY_LENGTH:        ctx->body.l             = td->t; break;
        case FOOTAG_BODY_WIDTH:         ctx->body.w             = td->t; break;
        case FOOTAG_BODY_HEIGHT:        ctx->body.h             = td->t; break;

        case FOOTAG_PADTYPE:            ctx->pad.type =
                                                TO_GEOM_PAD[ti->data.i]; break;
        case FOOTAG_PADPARAM:           ctx->pad.param          = td->d; break;
        case FOOTAG_LAND_GRID:          ctx->cfg.land.grid      = td->d; break;
        case FOOTAG_SILK_LINEW:         ctx->cfg.silk.linew     = td->d; break;
        case FOOTAG_SILK_GRID:          ctx->cfg.silk.grid      = td->d; break;
        case FOOTAG_ASSY_LINEW:         ctx->cfg.assy.linew     = td->d; break;
        case FOOTAG_ASSY_GRID:          ctx->cfg.assy.grid      = td->d; break;
        case FOOTAG_CYARD_LINEW:        ctx->cfg.cyard.linew    = td->d; break;
        case FOOTAG_CYARD_GRID:         ctx->cfg.cyard.grid     = td->d; break;

        case FOOTAG_FABTOL:             ctx->cfg.f              = td->d; break;
        case FOOTAG_PLACETOL:           ctx->cfg.p              = td->d; break;
        case FOOTAG_CYEXC:              ctx->cfg.cyexc          = td->d; break;
        case FOOTAG_PINNAMES:
                foot_stringlist_free(ctx->pinnames);
                if (td->sl) {
                        ctx->pinnames = foot_stringlist_clone(td->sl);
                        if (!ctx->pinnames) {
                                return FOOT_FAIL;
                        }
                }
                break;

        default:
                break;
        }
        return FOOT_OK;
}

static int dotagitems(
        struct foot_ctx *ctx,
        const struct footag_item *tagitems
)
{
        const struct footag_item *ti;
        for (ti = tagitems; ti->tag != FOOTAG_END; ti++) {
                const struct footag_desc *desc = footag_getdesc(ti->tag);
                if (desc) {
                        dbg(ctx, "%12s - %s\n", desc->name, desc->desc);
                }
                dotagitem(ctx, ti);
        }
        return FOOT_OK;
}

static const struct foot_ops *FOOTYPE_TO_OPS[FOOTYPE_NUM] = {
        [FOOTYPE_CHIP]          = &FOOT_OPS_CHIP_IPCB,
        [FOOTYPE_MOLDED]        = &FOOT_OPS_MOLDED_IPCB,
        [FOOTYPE_SOIC]          = &FOOT_OPS_SOIC_IPCB,
        [FOOTYPE_SOP]           = &FOOT_OPS_SOIC_IPCB,
        [FOOTYPE_SOT]           = &FOOT_OPS_SOIC_IPCB,
        [FOOTYPE_SIDECONCAVE]   = &FOOT_OPS_SIDECONCAVE_IPCB,
        [FOOTYPE_DIP]           = &FOOT_OPS_DIP_IPC,
        [FOOTYPE_BGA]           = &FOOT_OPS_BGA_IPCB,
        [FOOTYPE_PGA]           = &FOOT_OPS_PGA_IPC,
};

/* return: FOOTYPE or -1 */
static int getfootype(const struct footag_item *tagitems)
{
        const struct footag_item *typetag;
        if (!tagitems) {
                return -1;
        }
        typetag = footag_find(tagitems, FOOTAG_FOOTYPE);
        if (!typetag) {
                return -1;
        }
        return typetag->data.i;
}

/* NOTE: tags must have */
struct foot_ctx *foot_open(const struct footag_item *tagitems)
{
        int type;
        struct foot_ctx *ctx = NULL;
        const struct foot_ops *ops;

        type = getfootype(tagitems);
        if (type < 0) {
                return NULL;
        }

        ops = FOOTYPE_TO_OPS[type];
        if (!ops || !ops->size) {
                return NULL;
        }
        ctx = calloc(1, ops->size);
        if (!ctx) {
                return NULL;
        }
        ctx->foolog.mask = FOOLOG_DEFAULT;
        ctx->foolog.func = foolog_stderr;
        ctx->type = type;
        ctx->ops = ops;
        ctx->body = FOOT_BODY_DEFAULT;
        ctx->cfg = FOOT_CFG_DEFAULT;
        ctx->pad = ops->padspec;
        dotagitems(ctx, tagitems);
        ctx = ctx->ops->init(ctx, tagitems);
        /* NOTE: init() could have done realloc() on ctx */
        return ctx;
}

int foot_close(struct foot_ctx *ctx)
{
        ctx->ops->fini(ctx);
        foot_stringlist_free(ctx->pinnames);
        free(ctx);
        return FOOT_OK;
}

int foot_fini_none(struct foot_ctx *foot)
{
        UNUSED(foot);
        return FOOT_OK;
}

int foot_getbounds(
        struct foot_ctx *ctx,
        double *w,
        double *h
)
{
        if (w) { *w = ctx->cyard.x * 2; }
        if (h) { *h = ctx->cyard.y * 2; }
        return FOOT_OK;
}

struct footype_desc {
        const char *prefix;
        const char *desc;
};

static const struct footype_desc FOOTYPE_DESC[FOOTYPE_NUM] = {
        [FOOTYPE_CHIP]          = { .prefix = "CHIP",   },
        [FOOTYPE_MOLDED]        = { .prefix = "MOLDED", },
        [FOOTYPE_SOIC]          = { .prefix = "SOIC",   },
        [FOOTYPE_SOP]           = { .prefix = "SOP",    },
        [FOOTYPE_SOT]           = { .prefix = "SOT",    },
        [FOOTYPE_SIDECONCAVE]   = { .prefix = "SC",     },
        [FOOTYPE_DIP]           = { .prefix = "DIP",    },
        [FOOTYPE_BGA]           = { .prefix = "BGA",    },
        [FOOTYPE_PGA]           = { .prefix = "PGA",    },
};

char *foot_name(struct foot_ctx *ctx, const char *prefix, const char *suffix)
{
        if (prefix == NULL || *prefix == '\0') {
                prefix = FOOTYPE_DESC[ctx->type].prefix;
        }
        if (suffix == NULL) {
                suffix = "";
        }
        return ctx->ops->name(ctx, prefix, suffix);
}

char *foot_name_none(struct foot_ctx *ctx, const char *pre, const char *suf)
{
        dbg(ctx, "prefix=%s, suffix=%s\n", pre, suf);
        return NULL;
}

size_t foot_bodyprint_wlh(const struct foot_body *body, char *p, size_t len)
{
        size_t act;
        act = snprintf(
                p,
                len,
                "%.1fx%.1fx%.1f",
                body->w.nom,
                body->l.nom,
                body->h.nom
        );
        return act;
}

size_t foot_bodyprint_wl(const struct foot_body *body, char *p, size_t len)
{
        size_t act;
        act = snprintf(
                p,
                len,
                "%.1fx%.1f",
                body->w.nom,
                body->l.nom
        );
        return act;
}

int foot_smdlead_dotagitem(
        struct smdlead *lead,
        int tag,
        const union footag_data *td
)
{
        switch (tag) {
        case FOOTAG_LEAD_SPAN:          lead->span     = td->t; break;
        case FOOTAG_LEAD_LENGTH:        lead->length   = td->t; break;
        case FOOTAG_LEAD_WIDTH:         lead->width    = td->t; break;
        default: break;
        }
        return FOOT_OK;
}

char **foot_stringlist_clone(
        const char *const *name
)
{
        const char *const *src = name;
        char **dst;
        int count = 0;

        while (*src) {
                count++;
                src++;
        }
        dst = malloc((count + 1) * sizeof (const char *));
        if (!dst) {
                return NULL;
        }
        for (int i = 0; i < count; i++) {
                dst[i] = strdup(name[i]);
                if (!dst[i]) {
                        foot_stringlist_free(dst);
                        return NULL;
                }
        }
        dst[count] = NULL;
        return dst;
}

int foot_stringlist_free(
        char **list
)
{
        if (!list) return FOOT_OK;
        char **l = list;
        while (*l) {
                free(*l);
                l++;
        }
        free(list);
        return FOOT_OK;
}

int foot_stringlist_nelem(
        const char *const *name
)
{
        int count = 0;

        if (name) {
                while (*name) {
                        count++;
                        name++;
                }
        }
        return count;
}

char *const *foot_stringlist_find(
        char *const *list,
        const char *str
)
{
        if (!list) {
                return NULL;
        }
        while (*list) {
                if (0 == strcmp(str, *list)) {
                        return list;
                }
                list++;
        }
        return NULL;
}

/* Count occurrences of "-" in pinnames. */
int foot_nskip(struct foot_ctx *ctx)
{
        int n = 0;
        char *const *list = ctx->pinnames;
        while (NULL != (list = foot_stringlist_find(list, "-"))) {
                list++;
                n++;
        }
        return n;
}

