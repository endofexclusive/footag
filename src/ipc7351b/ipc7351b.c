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
#include <stddef.h>
#include <string.h>

#include "ipc7351b_priv.h"

struct tabledata {
        const char *ref;
        const char *desc;
        double toe, heel, side;
        double round;
        double cyexc;
};

typedef int (leadinfo_func)(
        const struct ipcb_comp *comp,
        double smin,
        enum ipcb_density density,
        struct tabledata *td
);

struct leadinfo {
        /* Special rules and deviations per lead type family. */
        leadinfo_func *func;
        const struct ipcbtable *table;
};

static const struct leadinfo LEADINFO[IPCB_LEADTYPE_NUM];

static int func_std(
        const struct ipcbtable *ipcbtable,
        enum ipcb_density density,
        struct tabledata *td
)
{
        td->desc        = ipcbtable->desc;
        td->toe         = ipcbtable->toe[density];
        td->heel        = ipcbtable->heel[density];
        td->side        = ipcbtable->side[density];
        td->round       = ipcbtable->round;
        td->cyexc       = ipcbtable->cyexc[density];

        return 0;
}

/* Chip components have two different tables, selected on dimensions. */
static int func_chip(
        const struct ipcb_comp *comp,
        double smin,
        enum ipcb_density density,
        struct tabledata *td
)
{
        UNUSED(smin);
        const struct ipcbtable *ipcbtable;
        ipcbtable = &ipcb_table3_5;
        if ((comp->lmax < 1.6) || (comp->wmax < 0.8)) {
                ipcbtable = &ipcb_table3_6;
        }
        return func_std(ipcbtable, density, td);
}

/* Molded have lead in opposite direction. */
static int func_molded(
        const struct ipcb_comp *comp,
        double smin,
        enum ipcb_density density,
        struct tabledata *td
)
{
        UNUSED(comp);
        UNUSED(smin);
        struct ipcbtable ipcbtable;

        ipcbtable = ipcb_table3_13;
        for (size_t i = 0; i < NELEM(ipcbtable.toe); i++) {
                double t;
                t = ipcbtable.heel[i];
                ipcbtable.heel[i] = ipcbtable.toe[i];
                ipcbtable.toe[i] = t;
        }
        return func_std(&ipcbtable, density, td);
}

static int func_gullwing(
        const struct ipcb_comp *comp,
        double smin,
        enum ipcb_density density,
        struct tabledata *td
)
{
        int ret;
        const struct ipcbtable *ipcbtable;
        ipcbtable = &ipcb_table3_2;
        if (comp->pitch <= 0.625) {
                ipcbtable = &ipcb_table3_3;
        }
        ret = func_std(ipcbtable, density, td);

        /*
         * Table 3-2 and Table 3-3, Note 1 and note 2.
         *
         * NOTE: Not implemented.
         * NOTE: Amax is not mentioned in IPC-7351B. It is in IPC-SM-782A.
         * NOTE: It is unknown what T1 means.
         */
        const double amax = 0.0;
        const double t1 = 0.6;
        if (
                0 &&
                smin <= amax &&
                t1 <= 0.5
        ) {
                static const double heelx[IPCB_DENSITY_NUM] = {
                        [IPCB_DENSITY_M] = 0.25,
                        [IPCB_DENSITY_N] = 0.15,
                        [IPCB_DENSITY_L] = 0.05
                };
                td->heel = heelx[density];
        }

        return ret;
}

static const struct leadinfo LEADINFO[IPCB_LEADTYPE_NUM] = {
        [IPCB_LEADTYPE_CHIP]            = { .func = func_chip, },
        [IPCB_LEADTYPE_MOLDED]          = { .func = func_molded, },
        [IPCB_LEADTYPE_GULLWING]        = { .func = func_gullwing, },
        [IPCB_LEADTYPE_SIDECONCAVE]     = { .table = &ipcb_table3_9, },
};

static int get_tabledata(
        enum ipcb_leadtype leadtype,
        const struct ipcb_comp *comp,
        double smin,
        enum ipcb_density density,
        struct tabledata *td
)
{
        const struct leadinfo *di;

        if (IPCB_LEADTYPE_NUM <= leadtype) {
                return 1;
        }
        di = &LEADINFO[leadtype];
        if (di->table) {
                return func_std(di->table, density, td);
        }
        if (di->func) {
                return di->func(comp, smin, density, td);
        }

        return 2;
}

/*
 * Calculate Euclidian distance from origin to (a,b,c)
 *
 * NOTE: This is NOT the root mean square as claimed in IPC-7351B.
 */
static double ed3(double a, double b, double c)
{
        return sqrt(a * a + b * b + c * c);
}

/* Potentially correct understanding of Sections 3.1.1 to 3.1.5. */
int ipcb_get_two(
        enum ipcb_leadtype leadtype,
        const struct ipcb_comp *comp,
        const struct ipcb_attr *attr,
        struct ipcb_twospec *spec
)
{
        int ret;
        struct tabledata td;
        double cl, cs, cw;
        double jt, jh, js;
        double smax_mod;
        double smin = comp->lmin - 2 * comp->tmax;
        double smax = comp->lmax - 2 * comp->tmin;

        {
                double ltol = comp->lmax - comp->lmin;
                double ttol = comp->tmax - comp->tmin;
                double wtol = comp->wmax - comp->wmin;
                double stol = smax - smin;
                double stol_ed3;

                stol_ed3 = ed3(ltol, ttol, ttol);
                smax_mod = smax - (stol - stol_ed3) / 2;

                cl = ltol;
                cs = stol_ed3;
                cw = wtol;
        }

        ret = get_tabledata(leadtype, comp, smin, attr->density, &td);
        if (ret) {
                return ret;
        }
        jt = td.toe;
        jh = td.heel;
        js = td.side;

        spec->zmax = comp->lmin + 2 * jt + ed3(cl, attr->f, attr->p);
        spec->gmin = smax_mod   - 2 * jh - ed3(cs, attr->f, attr->p);
        spec->xmax = comp->wmin + 2 * js + ed3(cw, attr->f, attr->p);

        spec->ref = td.ref;
        spec->desc = td.desc;
        spec->round = td.round;
        spec->cyexc = td.cyexc;

        return 0;
}

/*
 * IPC-7351B BGA calculations are confusing to me...
 */
static const char *LANDTOBALL_REF       =
        "Table 14-8";
static const char *LANDTOBALL_DESC      =
        "Land-to-Ball Calculations for "
        "Current and Future BGA Packages";
struct balltoland { double ball; double land; };
static const struct balltoland LANDTOBALL_BALLTOLAND[] = {
        {0.75, 0.60},
        {0.60, 0.50},
        {0.55, 0.45},
        {0.50, 0.45},
        {0.45, 0.40},
        {0.40, 0.35},
        {0.30, 0.25},
        {0.25, 0.20},
        {0.20, 0.20},
        {0.17, 0.20},
        {0.15, 0.18},
};

#define DENS_M IPCB_DENSITY_M
#define DENS_N IPCB_DENSITY_N
#define DENS_L IPCB_DENSITY_L
/*
 * Left for a future journey into IPC-7351B BGA land.
 */
#if 0
static const char *BGATABLE_REF  = "Table 3-17";
static const char *BGATABLE_DESC = "Ball Grid Array Components";
static const double BGATABLE_PERIC[IPCB_DENSITY_NUM] = {
        [DENS_M] = -0.25, [DENS_N] = -0.20, [DENS_L] = -0.15, };
static const double BGATABLE_PERIN[IPCB_DENSITY_NUM] = {
        [DENS_M] =  0.15, [DENS_N] =  0.10, [DENS_L] =  0.05, };
#endif
static const double BGATABLE_CYEXC[IPCB_DENSITY_NUM] = {
        [DENS_M] =  2.00, [DENS_N] =  1.00, [DENS_L] =  0.50, };
static const double BGATABLE_ROUND = 0.05;
#undef DENS_M
#undef DENS_N
#undef DENS_L

static double findbgaland(double lead)
{
        double land = 100000.0;
        double mindiff = 100000.0;
        const struct balltoland *btol = &LANDTOBALL_BALLTOLAND[0];

        for (size_t i = 0; i < NELEM(LANDTOBALL_BALLTOLAND); i++) {
                double diff = fabs(lead - btol->ball) / lead;
                if (diff < mindiff) {
                        land = btol->land;
                        mindiff = diff;
                }
                btol++;
        }

        return land;
}

int ipcb_get_bga(double diam, struct ipcb_bgaspec *spec)
{
        /* Density is an output and not an input !? */
        spec->density = IPCB_DENSITY_N;
        if (diam < (0.30 - 0.05 / 2)) {
                spec->density = IPCB_DENSITY_L;
        } else if ((0.60 + 0.16 / 2) <= diam) {
                spec->density = IPCB_DENSITY_M;
        }

        spec->ref       = LANDTOBALL_REF;
        spec->desc      = LANDTOBALL_DESC;
        spec->diam      = findbgaland(diam);
        spec->round     = BGATABLE_ROUND;
        spec->cyexc     = BGATABLE_CYEXC[spec->density];

        return 0;
}

