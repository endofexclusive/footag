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

#include <footag/footag.h>

#ifndef NELEM
 #define NELEM(a) ((sizeof a) / (sizeof (a[0])))
#endif

const int always_opt[] = {
        FOOTAG_BODY_HEIGHT,
        /* library configuration */
        FOOTAG_PADTYPE,
        FOOTAG_PADPARAM,
        FOOTAG_LAND_GRID,
        FOOTAG_SILK_LINEW,
        FOOTAG_SILK_GRID,
        FOOTAG_ASSY_LINEW,
        FOOTAG_ASSY_GRID,
        FOOTAG_CYARD_LINEW,
        FOOTAG_CYARD_GRID,
        FOOTAG_CYEXC,
        FOOTAG_END,
};

const int *footag_getalwaysopt(void)
{
        return &always_opt[0];
}

struct footag_info FINFO[] = {
        {
                .type   = FOOTYPE_CHIP,
                .tname  = "FOOTYPE_CHIP",
                .name   = "chip",
                .brief  = "chip components",
                .req    = (int []) {
                        FOOTAG_LEAD_LENGTH,
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PINNAMES,
                        FOOTAG_POLARIZED,
                        FOOTAG_DENSITY,
                        FOOTAG_FABTOL,
                        FOOTAG_PLACETOL,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_MOLDED,
                .tname  = "FOOTYPE_MOLDED",
                .name   = "molded",
                .brief  = "molded components",
                .req    = (int []) {
                        FOOTAG_LEAD_LENGTH,
                        FOOTAG_LEAD_WIDTH,
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PINNAMES,
                        FOOTAG_LEAD_SPAN,
                        FOOTAG_POLARIZED,
                        FOOTAG_DENSITY,
                        FOOTAG_FABTOL,
                        FOOTAG_PLACETOL,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_SOIC,
                .tname  = "FOOTYPE_SOIC",
                .name   = "soic",
                .brief  = "SOIC, SOP, SOT, and similar",
                .req    = (int []) {
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_LEADS,
                        FOOTAG_PITCH,
                        FOOTAG_LEAD_SPAN,
                        FOOTAG_LEAD_LENGTH,
                        FOOTAG_LEAD_WIDTH,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PINNAMES,
                        FOOTAG_DENSITY,
                        FOOTAG_FABTOL,
                        FOOTAG_PLACETOL,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_SIDECONCAVE,
                .tname  = "FOOTYPE_SIDECONCAVE",
                .name   = "sc",
                .brief  = "side concave SMD (oscillators)",
                .req    = (int []) {
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_LEADS,
                        FOOTAG_PITCH,
                        FOOTAG_LEAD_LENGTH,
                        FOOTAG_LEAD_WIDTH,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PINNAMES,
                        FOOTAG_LEAD_SPAN,
                        FOOTAG_DENSITY,
                        FOOTAG_FABTOL,
                        FOOTAG_PLACETOL,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_DIP,
                .tname  = "FOOTYPE_DIP",
                .name   = "dip",
                .brief  = "DIP, through-hole",
                .req    = (int []) {
                        FOOTAG_PINNAMES,
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_LEADS,
                        FOOTAG_PITCH,
                        FOOTAG_LEAD_DIAM,
                        FOOTAG_LEAD_DIST,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_DENSITY,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_BGA,
                .tname  = "FOOTYPE_BGA",
                .name   = "BGA",
                .brief  = "SMD array components",
                .req    = (int []) {
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_PITCH,
                        FOOTAG_ROWS,
                        FOOTAG_COLS,
                        FOOTAG_LEAD_WIDTH,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PROWS,
                        FOOTAG_PCOLS,
                        FOOTAG_ROWNAMES,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
        {
                .type   = FOOTYPE_PGA,
                .tname  = "FOOTYPE_PGA",
                .name   = "PGA",
                .brief  = "pin grid array, through-hole",
                .req    = (int []) {
                        FOOTAG_BODY_LENGTH,
                        FOOTAG_BODY_WIDTH,
                        FOOTAG_PITCH,
                        FOOTAG_ROWS,
                        FOOTAG_COLS,
                        FOOTAG_LEAD_DIAM,
                        FOOTAG_END,
                },
                .opt    = (int []) {
                        FOOTAG_PROWS,
                        FOOTAG_PCOLS,
                        FOOTAG_ROWNAMES,
                        FOOTAG_DENSITY,
                        FOOTAG_END,
                },
                .next   = NULL,
        },
};

const char *FOOTAG_TYPE_STRING[FOOTAG_TYPE_NUM] = {
        [FOOTAG_TYPE_NONE]      = "none",
        [FOOTAG_TYPE_I]         = "integer",
        [FOOTAG_TYPE_D]         = "float",
        [FOOTAG_TYPE_S]         = "string",
        [FOOTAG_TYPE_P]         = "pointer",
        [FOOTAG_TYPE_SL]        = "string list",
        [FOOTAG_TYPE_F]         = "function",
        [FOOTAG_TYPE_T]         = "tolerance",
};

#define GEN(n, t, d) [FOOTAG_##n] = { .name = #n, FOOTAG_TYPE_##t, d}
const struct footag_desc FOOTAG_DESC[FOOTAG_NUM] = {
        GEN(FOOTYPE,            I,      "footprint type"),
        GEN(BODY_LENGTH,        T,      "body length"),
        GEN(BODY_WIDTH,         T,      "body width"),
        GEN(BODY_HEIGHT,        T,      "body height"),
        GEN(LEADS,              I,      "number of leads"),
        GEN(PITCH,              D,      "lead pitch"),
        GEN(ROWS,               I,      "number of rows"),
        GEN(COLS,               I,      "number of columns"),
        GEN(PROWS,              I,      "number of perimeter rows"),
        GEN(PCOLS,              I,      "number of perimeter columns"),
        /* As a special case, the name "-" skips the pin. */
        GEN(PINNAMES,           SL,     "pin names"),
        GEN(ROWNAMES,           SL,     "row names"),
        GEN(LEAD_SPAN,          T,      "lead span"),
        GEN(LEAD_LENGTH,        T,      "lead length"),
        GEN(LEAD_WIDTH,         T,      "lead width"),
        GEN(LEAD_DIAM,          T,      "lead diameter"),
        GEN(LEAD_DIST,          T,      "lead distance"),
        GEN(POLARIZED,          I,      "polarized"),
        /* FOOTAG_PADTYPE_ */
        GEN(PADTYPE,            I,      "pad type" ),
        GEN(PADPARAM,           D,      "pad rect radius" ),
        GEN(LAND_GRID,          D,      "land grid"),
        GEN(SILK_LINEW,         D,      "silkscreen line width"),
        GEN(SILK_GRID,          D,      "silkscreen grid"),
        GEN(ASSY_LINEW,         D,      "assembly line width"),
        GEN(ASSY_GRID,          D,      "assembly grid"),
        GEN(CYARD_LINEW,        D,      "courtyard line width"),
        GEN(CYARD_GRID,         D,      "courtyard grid"),
        GEN(CYEXC,              D,      "courtyard excess"),
        /* IPC-7351B "Land protrusion". Represented by FOOTAG_LEVEL_ */
        GEN(DENSITY,            I,      "density level"),
        GEN(FABTOL,             D,      "fabrication tol. (\"f\") (IPC-7351B)"),
        GEN(PLACETOL,           D,      "part placement tol. (\"p\") (IPC-7351B)"),
};
#undef GEN

const struct footag_desc *footag_getdesc(int tag)
{
        if (tag < 0 || FOOTAG_NUM <= tag) {
                return NULL;
        }
        return &FOOTAG_DESC[tag];
}

const struct footag_item *footag_find(
        const struct footag_item *tagitems,
        int tag
)
{
        while (tagitems->tag != FOOTAG_END) {
                if (tagitems->tag == tag) {
                        return tagitems;
                }
                tagitems++;
        }
        return NULL;
}

static struct footag_info *magic = NULL;

const struct footag_info *footag_getinfo(void)
{
        if (magic) {
                return magic;
        }

        const size_t ninfos = NELEM(FINFO);

        for (size_t i = 0; i < ninfos-1; i++) {
                FINFO[i].next = &FINFO[i+1];
        }
        FINFO[ninfos-1].next = NULL;

        magic = &FINFO[0];

        return magic;
}

