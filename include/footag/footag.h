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

#ifndef FOOTAG_FOOTAG_H
#define FOOTAG_FOOTAG_H

#include <stddef.h>
#include <footag/footol.h>

/* GENERAL */

union footag_data {
        int i;
        double d;
        char *s;
        void *p;
        const char *const *sl;
        void (*f)(void);
        struct footol t;
};

enum {
        FOOTAG_TYPE_NONE,
        FOOTAG_TYPE_I,
        FOOTAG_TYPE_D,
        FOOTAG_TYPE_S,
        FOOTAG_TYPE_P,
        /* NULL terminated array of string pointers. (const char *rownames[]) */
        FOOTAG_TYPE_SL,
        FOOTAG_TYPE_F,
        FOOTAG_TYPE_T,
        FOOTAG_TYPE_NUM,
};
extern const char *FOOTAG_TYPE_STRING[FOOTAG_TYPE_NUM];

struct footag_item {
        int tag;
        union footag_data data;
};

/*
 * Three different levels of something, which can be used where appropriate,
 * for example in IPCB-7351 and IPCB-7251.
 */
enum {
        FOOTAG_LEVEL_0 = 0, FOOTAG_LEVEL_1, FOOTAG_LEVEL_2,
        FOOTAG_LEVEL_A = 0, FOOTAG_LEVEL_B, FOOTAG_LEVEL_C,
        /* most             nominal         least */
        FOOTAG_LEVEL_M = 0, FOOTAG_LEVEL_N, FOOTAG_LEVEL_L,
        FOOTAG_LEVEL_NUM = 3,
};

enum {
        /* SMD rectangular */
        FOOTAG_PADTYPE_RECT,
        /* SMD rounded rectangular */
        FOOTAG_PADTYPE_RRECT,
        /* SMD circular */
        FOOTAG_PADTYPE_CIRC,
        /* Through-hole rectangular */
        FOOTAG_PADTYPE_TRECT,
        /* Through-hole circular */
        FOOTAG_PADTYPE_TCIRC,
        FOOTAG_PADTYPE_NUM,
};

/* Values for footag_item->tag */
enum {
        FOOTAG_END,
        /* component definition */
        FOOTAG_FOOTYPE,
        FOOTAG_BODY_LENGTH,
        FOOTAG_BODY_WIDTH,
        FOOTAG_BODY_HEIGHT,
        FOOTAG_LEADS,
        FOOTAG_PITCH,
        FOOTAG_ROWS,
        FOOTAG_COLS,
        FOOTAG_PROWS,
        FOOTAG_PCOLS,
        FOOTAG_PINNAMES,
        FOOTAG_ROWNAMES,
        FOOTAG_LEAD_SPAN,
        FOOTAG_LEAD_LENGTH,
        FOOTAG_LEAD_WIDTH,
        FOOTAG_LEAD_DIAM,
        FOOTAG_LEAD_DIST,
        FOOTAG_POLARIZED,

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

        /* IPC-7351B */
        FOOTAG_DENSITY,
        FOOTAG_FABTOL,
        FOOTAG_PLACETOL,
        FOOTAG_CYEXC,

        FOOTAG_NUM,

        FOOTAG_CUSTOM0 = 0x100,
};

struct footag_desc {
        const char *name;
        int type;
        const char *desc;
};
/* Description of all tags, for use by application. */
extern const struct footag_desc FOOTAG_DESC[FOOTAG_NUM];
/* return: Description if 0 <= tag < FOOTAG_NUM, else NULL */
const struct footag_desc *footag_getdesc(int tag);

enum footype {
        FOOTYPE_CHIP,
        FOOTYPE_MOLDED,
        FOOTYPE_SOIC,
        FOOTYPE_SOP,
        FOOTYPE_SOT,
        FOOTYPE_SIDECONCAVE,
        FOOTYPE_DIP,
        FOOTYPE_BGA,
        FOOTYPE_PGA,
        FOOTYPE_NUM,
};

struct footag_info {
        enum footype type;
        char *name;
        char *tname;
        char *brief;
        /* required tags */
        int *req;
        /* optional tags */
        int *opt;
        struct footag_info *next;
};

/*
 * Retrieve a linked list of footprint type infos.
 */
const struct footag_info *footag_getinfo(void);

/*
 * Retrieve a list of tags which are always optional. The list is terminated
 * with FOOTAG_END
 */
const int *footag_getalwaysopt(void);

/* Find first tagitem matching tag in a taglist. */
const struct footag_item *footag_find(
        const struct footag_item *tagitems,
        int tag
);

#endif

