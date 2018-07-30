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

#include "fout_priv.h"

int fout_close(struct fout_ctx *ctx)
{
        return ctx->ops->fini(ctx);
}

static int dolayer(
        struct fout_ctx *ctx,
        const struct geom_shape *head,
        int layer
)
{
        int ret;
        const struct geom_shape *shape;

        ctx->ops->prelayer(ctx, layer);
        for (shape = head; shape; shape = shape->next) {
                if (shape->layer != layer) {
                        continue;
                }
                ret = ctx->ops->doshape(ctx, shape);
                if (ret) { return ret; }
        }
        ctx->ops->postlayer(ctx, layer);

        return FOUT_OK;
}

int fout_gen(
        struct fout_ctx *ctx,
        const struct geom_ctx *geom,
        const char *name
)
{
        int ret;
        const struct geom_shape *head;

        ret = geom_getshapelist(geom, &head);
        if (ret) { return FOUT_FAIL; }

        ret = ctx->ops->setname(ctx, name);
        if (ret) { return ret; }
        ret = ctx->ops->pregen(ctx, head);
        if (ret) { return ret; }

        ret = dolayer(ctx, head, GEOM_LAYER_CYARD);
        if (ret) { return ret; }
        ret = dolayer(ctx, head, GEOM_LAYER_COPPER);
        if (ret) { return ret; }
        ret = dolayer(ctx, head, GEOM_LAYER_SILK);
        if (ret) { return ret; }
        ret = dolayer(ctx, head, GEOM_LAYER_ASSY);
        if (ret) { return ret; }

        ret = ctx->ops->postgen(ctx, head);
        if (ret) { return ret; }

        return FOUT_OK;
}

int fout_setname_none(struct fout_ctx *ctx, const char *name)
{
        UNUSED(ctx); UNUSED(name); return FOUT_OK;
}
int fout_gen_none(struct fout_ctx *ctx, const struct geom_shape *head)
{
        UNUSED(ctx); UNUSED(head); return FOUT_OK;
}
int fout_layer_none(struct fout_ctx *ctx, int layer)
{
        UNUSED(ctx); UNUSED(layer); return FOUT_OK;
}

