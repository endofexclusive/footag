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

static int calcpads(struct foot_ctx *foot)
{
        return foot_twoside_ipcb_calcpads(foot, IPCB_LEADTYPE_MOLDED);
}

static int dotagitems(struct soic_ctx *ctx, const struct footag_item *tagitems)
{
        const struct footag_item *ti;
        int hasleadspan = 0;
        for (ti = tagitems; ti->tag != FOOTAG_END; ti++) {
                foot_smdlead_dotagitem(&ctx->lead, ti->tag, &ti->data);
                if (ti->tag == FOOTAG_POLARIZED) {
                        ctx->two.comp.pol = ti->data.i;
                } else if (ti->tag == FOOTAG_LEAD_SPAN) {
                        hasleadspan = 1;
                } else if (ti->tag == FOOTAG_DENSITY) {
                        ctx->density = FOOT_LEVEL_TO_IPCB_DENSITY[ti->data.i];
                }
        }
        /* reasonable to assume that span is component width */
        if (!hasleadspan) {
                ctx->lead.span = ctx->two.foot.body.w;
        }
        return FOOT_OK;
}

static struct foot_ctx *init(
        struct foot_ctx *foot,
        const struct footag_item *tagitems
)
{
        struct soic_ctx *ctx = (struct soic_ctx *) foot;
        ctx->two.comp = TWOROW_COMP_DEFAULT_2PIN;
        ctx->density = IPCB_DENSITY_N;
        dotagitems(ctx, tagitems);
        return foot;
}

const struct foot_ops FOOT_OPS_MOLDED_IPCB = {
        .padspec        = { .type = GEOM_PAD_RRECT, .param = 0.1, },
        .calcpads       = calcpads,
        .drawpads       = foot_tworow_drawpads,
        .setpad         = foot_setpad_default,
        .drawsilk       = foot_tworow_drawsilk,
        .drawassy       = foot_tworow_drawassy_golden_or_rect,
        .calccyard      = foot_calccyard_ipcb,
        .drawcyard      = foot_drawcyard_ipcb,
        .name           = foot_twoside_ipcb_chip_name,
        .init           = init,
        .fini           = foot_fini_none,
        .size           = sizeof(struct soic_ctx),
};

