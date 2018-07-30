#include <stdlib.h>
#include <stdio.h>
#include "futil.h"

#ifndef OUTDIR
 #define OUTDIR "random-out/"
#endif

#ifndef NELEM
 #define NELEM(a) ((sizeof a) / (sizeof (a[0])))
#endif

struct compdim {
        double bw, bl, btol;
        double lw, ll, ltol;
        double ls, lstol;
};

static const int RECT_RRECT[2] = { FOOTAG_PADTYPE_RECT, FOOTAG_PADTYPE_RRECT, };

/* Return a number in range [min,max). */
static double frand(double min, double max)
{
        double ret;
        int v = rand();
        ret = min + (max - min) * v / RAND_MAX;
        return ret;
}

static int gencfg(struct footag_item *tis) {
        int i = 0;
        tis[i].tag      = FOOTAG_SILK_LINEW,
        tis[i].data.d   = frand(0.08, 0.15);
        i++;
        tis[i].tag      = FOOTAG_ASSY_LINEW,
        tis[i].data.d   = frand(0.08, 0.15);
        i++;
        tis[i].tag      = FOOTAG_DENSITY;
        tis[i].data.i   = rand() % 3;
        i++;

        return i;
}

static const struct compdim chipdim[] = {
        { .bw = 1.0,    .bl = 0.5,      .btol = 0.1 },
        { .bw = 1.6,    .bl = 0.8,      .btol = 0.2 },
        { .bw = 2.8,    .bl = 1.3,      .btol = 0.3 },
        { .bw = 3.2,    .bl = 2.5,      .btol = 0.4 },
};

static int genchip(struct footag_item *tis) {
        int i = 0;
        const struct compdim *dim = &chipdim[rand() % NELEM(chipdim)];
        tis[i].tag      = FOOTAG_FOOTYPE;
        tis[i].data.i   = FOOTYPE_CHIP;
        i++;
        tis[i].tag      = FOOTAG_BODY_WIDTH;
        tis[i].data.t   = footol_auto(dim->bw, dim->btol);
        i++;
        tis[i].tag      = FOOTAG_BODY_LENGTH;
        tis[i].data.t   = footol_auto(dim->bl, dim->btol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_LENGTH;
        tis[i].data.t   = footol_auto(dim->bw*(frand(0.15, 0.20)), dim->btol);
        i++;
        tis[i].tag      = FOOTAG_PADTYPE;
        tis[i].data.i   = RECT_RRECT[rand() % 2];
        i++;

        i += gencfg(&tis[i]);
        tis[i].tag      = FOOTAG_END;
        i++;

        return i;
}

/* From AVX data sheet */
static const struct compdim moldeddim[] = {
        {
                .bw = 3.2, .bl = 1.6, .btol = 0.2,
                .ll = 0.8, .lw = 1.2, .ltol = 0.1,
        },
        {
                .bw = 3.5, .bl = 2.8, .btol = 0.2,
                .ll = 0.8, .lw = 2.2, .ltol = 0.1,
        },
        {
                .bw = 6.0, .bl = 3.2, .btol = 0.2,
                .ll = 1.3, .lw = 2.2, .ltol = 0.1,
        },
        {
                .bw = 7.3, .bl = 4.3, .btol = 0.2,
                .ll = 1.3, .lw = 2.4, .ltol = 0.1,
        },
        {
                .bw = 7.3, .bl = 6.1, .btol = 0.2,
                .ll = 1.3, .lw = 3.1, .ltol = 0.1,
        },
};

static int genmolded(struct footag_item *tis) {
        int i = 0;
        const struct compdim *dim = &moldeddim[rand() % NELEM(moldeddim)];
        tis[i].tag      = FOOTAG_FOOTYPE;
        tis[i].data.i   = FOOTYPE_MOLDED;
        i++;
        tis[i].tag      = FOOTAG_BODY_WIDTH;
        tis[i].data.t   = footol_auto(dim->bw, dim->btol);
        i++;
        tis[i].tag      = FOOTAG_BODY_LENGTH;
        tis[i].data.t   = footol_auto(dim->bl, dim->btol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_LENGTH;
        tis[i].data.t   = footol_auto(dim->ll*(frand(0.80, 1.20)), dim->ltol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_WIDTH;
        tis[i].data.t   = footol_auto(dim->lw*(frand(0.80, 1.20)), dim->ltol);
        i++;
        tis[i].tag      = FOOTAG_PADTYPE;
        tis[i].data.i   = RECT_RRECT[rand() % 2];
        i++;
        tis[i].tag      = FOOTAG_POLARIZED;
        tis[i].data.i   = rand() % 2;
        i++;

        i += gencfg(&tis[i]);
        tis[i].tag      = FOOTAG_END;
        i++;

        return i;
}

static const struct compdim soicdim[] = {
        {
                .bw = 5.28,  .bl = 5.28, .btol = 0.1,
                .ll = 0.63,  .lw = 0.42, .ltol = 0.06,
                .ls = 8.00,  .lstol = 0.33,
        },
};

static int gensoic(struct footag_item *tis) {
        int i = 0;
        const struct compdim *dim = &soicdim[rand() % NELEM(soicdim)];
        int nleads = 6 + 2 * (rand() % 10);
        double pitch = 1.27;
        tis[i].tag      = FOOTAG_FOOTYPE;
        tis[i].data.i   = FOOTYPE_SOIC;
        i++;
        tis[i].tag      = FOOTAG_BODY_WIDTH;
        tis[i].data.t   = footol_auto(dim->bw, dim->btol);
        i++;
        tis[i].tag      = FOOTAG_BODY_LENGTH;
        tis[i].data.t   = footol_auto(dim->bl + pitch * ((nleads-8)/2), dim->btol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_LENGTH;
        tis[i].data.t   = footol_auto(dim->ll*(frand(0.90, 1.10)), dim->ltol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_WIDTH;
        tis[i].data.t   = footol_auto(dim->lw*(frand(0.80, 1.20)), dim->ltol);
        i++;
        tis[i].tag      = FOOTAG_LEAD_SPAN;
        tis[i].data.t   = footol_auto(dim->ls*(frand(0.90, 1.10)), dim->lstol);
        i++;
        tis[i].tag      = FOOTAG_PITCH;
        tis[i].data.d   = pitch;
        i++;
        tis[i].tag      = FOOTAG_LEADS;
        tis[i].data.i   = nleads;
        i++;
        tis[i].tag      = FOOTAG_PADTYPE;
        tis[i].data.i   = RECT_RRECT[rand() % 2];
        i++;

        i += gencfg(&tis[i]);
        tis[i].tag      = FOOTAG_END;
        i++;

        return i;
}

static int genbga(struct footag_item *tis) {
        int i = 0;
        int rows = 2 + 2 * (rand() % 15);
        int cols = 2 + 2 * (rand() % 15);
        double pitch = frand(0.8, 1.2);
        tis[i].tag      = FOOTAG_FOOTYPE;
        tis[i].data.i   = FOOTYPE_BGA;
        i++;
        tis[i].tag      = FOOTAG_BODY_WIDTH;
        tis[i].data.t   = footol_auto(pitch * (1 + cols), 0.40);
        i++;
        tis[i].tag      = FOOTAG_BODY_LENGTH;
        tis[i].data.t   = footol_auto(pitch * (1 + rows), 0.40);
        i++;
        tis[i].tag      = FOOTAG_ROWS;
        tis[i].data.i   = rows;
        i++;
        tis[i].tag      = FOOTAG_COLS;
        tis[i].data.i   = cols;
        i++;
        tis[i].tag      = FOOTAG_LEAD_WIDTH;
        tis[i].data.t   = footol_auto((frand(0.40, 0.60)), 0.1);
        i++;
        tis[i].tag      = FOOTAG_PITCH;
        tis[i].data.d   = pitch;
        i++;

        i += gencfg(&tis[i]);
        tis[i].tag      = FOOTAG_END;
        i++;

        return i;
}

static int genpga(struct footag_item *tis) {
        int i = 0;
        int rows = 2 + 2 * (rand() % 10);
        int cols = 2 + 2 * (rand() % 10);
        int prows = (rand() % (1 + rows/2)) * (rand() % 2);
        int pcols = (rand() % (1 + cols/2)) * (rand() % 2);
        double pitchi = frand(0.08, 0.14);
        tis[i].tag      = FOOTAG_FOOTYPE;
        tis[i].data.i   = FOOTYPE_PGA;
        i++;
        tis[i].tag      = FOOTAG_BODY_WIDTH;
        tis[i].data.t   = footol_autoi(pitchi * (0.06 + cols), 0.02);
        i++;
        tis[i].tag      = FOOTAG_BODY_LENGTH;
        tis[i].data.t   = footol_autoi(pitchi * (0.06 + rows), 0.02);
        i++;
        tis[i].tag      = FOOTAG_ROWS;
        tis[i].data.i   = rows;
        i++;
        tis[i].tag      = FOOTAG_COLS;
        tis[i].data.i   = cols;
        i++;
        tis[i].tag      = FOOTAG_PROWS;
        tis[i].data.i   = prows;
        i++;
        tis[i].tag      = FOOTAG_PCOLS;
        tis[i].data.i   = pcols;
        i++;
        tis[i].tag      = FOOTAG_LEAD_DIAM;
        tis[i].data.t   = footol_autoi((frand(0.012, 0.020)), 0.02);
        i++;
        tis[i].tag      = FOOTAG_PITCH;
        tis[i].data.d   = footol_inchtomm(pitchi);
        i++;

        i += gencfg(&tis[i]);
        tis[i].tag      = FOOTAG_END;
        i++;

        return i;
}

int main(void)
{
        const int NFOOT = 2000;
        struct footag_item tis[FOOTAG_NUM];
        int n = 0;

        srand(1);

        for (int i = 0; i < NFOOT/5; i++, n++) {
                genchip(&tis[0]);
                dosimple(tis, OUTDIR);
        }

        for (int i = 0; i < NFOOT/5; i++, n++) {
                genmolded(&tis[0]);
                dosimple(tis, OUTDIR);
        }

        for (int i = 0; i < NFOOT/5; i++, n++) {
                gensoic(&tis[0]);
                dosimple(tis, OUTDIR);
        }

        for (int i = 0; i < NFOOT/50; i++, n++) {
                genbga(&tis[0]);
                dosimple(tis, OUTDIR);
        }

        for (int i = 0; i < NFOOT/50; i++, n++) {
                genpga(&tis[0]);
                dosimple(tis, OUTDIR);
        }

        printf("Created %d footprints in directory %s\n", n, OUTDIR);

        return 0;
}

