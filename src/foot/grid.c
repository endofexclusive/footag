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

#include "grid_priv.h"

#define dbg(ctx, ...) foolog_msg( \
        FOOLOG_LEVEL_DEBUG, \
        &ctx->foot.foolog, \
        __VA_ARGS__ \
)

static inline double padxpos(const struct grid_ctx *ctx, int col)
{
        assert(0 <= col && col < ctx->comp.cols);
        int cols = ctx->comp.cols;
        double pitch = ctx->comp.pitch;
        double x;
        x = (cols / 2.0) - 1.0 / 2 - col;
        x = -x;
        x *= pitch;
        x = landsnap(&ctx->foot, x);
        return x;
}

static inline double padypos(const struct grid_ctx *ctx, int row)
{
        assert(0 <= row && row < ctx->comp.rows);
        int rows = ctx->comp.rows;
        double pitch = ctx->comp.pitch;
        double y;
        y = (rows / 2.0) - 1.0 / 2 - row;
        y *= pitch;
        y = landsnap(&ctx->foot, y);
        return y;
}

static const char *const ROWNAMES[] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P",
        "R", "T", "U", "V", "W", "Y", "AA", "AB", "AC", "AD", "AE", "AF",
        "AG", "AH", "AJ", "AK", "AL", "AM", "AN", "AP", "AR", "AT", "AU",
};

/*
 * NOTE: This is independent of pad size, package, standard, etc.
 * NOTE: This is specific to grid type footprints.
 */
int foot_grid_drawpads(struct foot_ctx *foot, struct geom_ctx *g)
{
        struct grid_ctx *ctx = (struct grid_ctx *) foot;
        int rows = ctx->comp.rows;
        int cols = ctx->comp.cols;
        int prows = ctx->comp.prows;
        int pcols = ctx->comp.pcols;
        int ndrawn = 0;
        const char *const *rowname = ROWNAMES;

        if (ctx->comp.rownames) {
                rowname = (const char *const *) ctx->comp.rownames;
                /* Don't punish us for user limitations */
                if (foot_stringlist_nelem(rowname) < rows) {
                        return FOOT_FAIL;
                }
        } else {
                /* Don't punish user for our limitations */
                if ((int) NELEM(ROWNAMES) < rows) {
                        return FOOT_FAIL;
                }
        }
        for (int row = 0; row < rows; row++) {
                double y = padypos(ctx, row);
                for (int col = 0; col < cols; col++) {
                        if (
                                ((rows-prows)/2 <= row) &&
                                (row < (rows - (rows - prows) / 2)) &&
                                ((cols-pcols)/2 <= col) &&
                                (col < (cols - (cols - pcols) / 2))
                        ) {
                                continue;
                        }
                        int ret;
                        double x = padxpos(ctx, col);
                        char name[PINNAME_BUFSIZE];
                        ret = snprintf(
                                name,
                                NELEM(name),
                                "%s%d",
                                rowname[row],
                                col+1
                        );
                        if ((int) NELEM(name) <= ret) {
                                return FOOT_FAIL;
                        }
                        geom_moveto(g, x, y);
                        dbg(ctx, "pad %4s at %f, %f\n", name, x, y);
                        ctx->foot.ops->setpad(&ctx->foot, g, ndrawn);
                        geom_padc(g, name);
                        ndrawn++;
                }
        }

        extrpad(ctx)->x = fabs(padxpos(ctx, 0)) + ctx->foot.pad.w / 2;
        extrpad(ctx)->y = padypos(ctx, 0) + ctx->foot.pad.h / 2;

        return FOOT_OK;
}

int foot_grid_dotagitem(struct grid_ctx *ctx, const struct footag_item *ti)
{
        const union footag_data *td = &ti->data;
        switch (ti->tag) {
        case FOOTAG_PITCH:              ctx->comp.pitch         = td->d; break;
        case FOOTAG_ROWS:               ctx->comp.rows          = td->i; break;
        case FOOTAG_COLS:               ctx->comp.cols          = td->i; break;
        case FOOTAG_PROWS:              ctx->comp.prows         = td->i; break;
        case FOOTAG_PCOLS:              ctx->comp.pcols         = td->i; break;
        case FOOTAG_ROWNAMES:
                foot_stringlist_free(ctx->comp.rownames);
                if (td->sl) {
                        ctx->comp.rownames = foot_stringlist_clone(td->sl);
                        if (!ctx->comp.rownames) {
                                return FOOT_FAIL;
                        }
                }
                break;
        default: break;
        }
        return FOOT_OK;
}

