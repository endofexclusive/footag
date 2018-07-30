#include "futil.h"

#ifndef OUTDIR
 #define OUTDIR "myproj-out/"
#endif

int main(void)
{
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_CHIP },
                { NAME_PREFIX,          .data.s = "RESC" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 1.60,  0.20) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 0.80,  0.20) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 0.45,  0.10) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.30,  0.20) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_CHIP },
                { NAME_PREFIX,          .data.s = "CAPC" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 1.60,  0.15) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 0.80,  0.15) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 0.80,  0.10) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.35,  0.15) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_CHIP },
                { NAME_PREFIX,          .data.s = "CAPC" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 3.20,  0.40) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 2.50,  0.30) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 2.50,  0.30) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.60,  0.30) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_MOLDED },
                { NAME_PREFIX,          .data.s = "CAPMP" },
                { NAME_SUFFIX,          .data.s = "" },
                { FOOTAG_POLARIZED,     .data.i = 1 },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 7.30,  0.30) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 4.30,  0.30) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 2.80,  0.30) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 1.30,  0.30) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 2.40,  0.10) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOP },
                { NAME_PREFIX,          .data.s = "SOP" },
                { NAME_SUFFIX,          .data.s = "ISSI_SDRAM" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto(10.16,  0.13) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto(22.22,  0.20) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.20,  0.00) },
                { FOOTAG_LEADS,         .data.i = 54 },
                { FOOTAG_PITCH,         .data.d = 0.80 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto(11.76,  0.20) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.50,  0.10) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.375, 0.075) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOP },
                { NAME_PREFIX,          .data.s = "SOP" },
                { NAME_SUFFIX,          .data.s = "CDCVF2505" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 4.40,  0.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 3.00,  0.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.20,  0.00) },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 0.65 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 6.400,  0.200) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.625,  0.125) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.245,  0.055) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOP },
                { NAME_PREFIX,          .data.s = "SOP" },
                { NAME_SUFFIX,          .data.s = "CDCVF2505" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 4.40,  0.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 3.00,  0.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.20,  0.00) },
                { FOOTAG_DENSITY,       .data.i = FOOTAG_LEVEL_M },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 0.65 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 6.400,  0.200) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.625,  0.125) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.245,  0.055) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOP },
                { NAME_PREFIX,          .data.s = "SOP" },
                { NAME_SUFFIX,          .data.s = "CDCVF2505" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 4.40,  0.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 3.00,  0.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.20,  0.00) },
                { FOOTAG_DENSITY,       .data.i = FOOTAG_LEVEL_L },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 0.65 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 6.400,  0.200) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.625,  0.125) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.245,  0.055) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOIC },
                { NAME_PREFIX,          .data.s = "SOIC" },
                { NAME_SUFFIX,          .data.s = "CDCVF2505" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 3.90,  0.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 4.90,  0.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.75,  0.00) },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 1.27 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 5.80,  6.20) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.40,  1.27) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.31,  0.51) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOIC },
                { NAME_PREFIX,          .data.s = "SOIC" },
                { NAME_SUFFIX,          .data.s = "S25FL127S" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 5.28,  0.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 5.28,  0.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.75,  2.16) },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 1.27 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 8.00,  0.33) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.51,  0.76) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.36,  0.48) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOT },
                { NAME_PREFIX,          .data.s = "SOT" },
                { NAME_SUFFIX,          .data.s = "TLV73318DPBV" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 1.45,  1.75) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 2.75,  3.05) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.45,  1.45) },
                { FOOTAG_LEADS,         .data.i = 6 },
                { FOOTAG_PITCH,         .data.d = 0.95 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 2.60,  3.00) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.30,  0.60) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.30,  0.50) },
                { FOOTAG_PINNAMES,      .data.sl = (const char *const []) {
                                "1", "2", "3", "4", "-", "5", NULL
                        }
                },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SOP },
                { NAME_PREFIX,          .data.s = "SOP" },
                { NAME_SUFFIX,          .data.s = "TPS22958DGK" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 2.90,  3.10) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 2.90,  3.10) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.10,  0.00) },
                { FOOTAG_LEADS,         .data.i = 8 },
                { FOOTAG_PITCH,         .data.d = 0.65 },
                { FOOTAG_LEAD_SPAN,     .data.t = footol_auto( 4.75,  5.05) },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 0.40,  0.70) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.25,  0.38) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_BGA },
                { NAME_PREFIX,          .data.s = "BGA" },
                { NAME_SUFFIX,          .data.s = "Artix-7" },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto(23.00,  0.40) },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto(23.00,  0.40) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 2.20,  0.20) },
                { FOOTAG_ROWS,          .data.i = 22 },
                { FOOTAG_COLS,          .data.i = 22 },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 0.60,  0.10) },
                { FOOTAG_PITCH,         .data.d = 1.00 },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_PGA },
                { NAME_PREFIX,          .data.s = "PGA" },
                { NAME_SUFFIX,          .data.s = "MC68060" },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_autoi( 1.84,  1.88) },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_autoi( 1.84,  1.88) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_autoi( 0.11,  0.14) },
                { FOOTAG_ROWS,          .data.i = 18 },
                { FOOTAG_COLS,          .data.i = 18 },
                { FOOTAG_PROWS,         .data.i = 10 },
                { FOOTAG_PCOLS,         .data.i = 10 },
                { FOOTAG_ROWNAMES,      .data.sl = (const char *const []) {
                                "A", "B", "C", "D", "E", "F", "G", "H", "J",
                                "K", "L", "M", "N", "P", "Q", "R", "S", "T",
                                NULL
                        }
                },
                { FOOTAG_LEAD_DIAM,     .data.t = footol_autoi( 0.016,  0.02) },
                { FOOTAG_PITCH,         .data.d = footol_inchtomm(0.10) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_DIP },
                { NAME_PREFIX,          .data.s = "DIP" },
                { NAME_SUFFIX,          .data.s = "PTH04070W" },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_autoi( 0.495,  0.010) },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_autoi( 0.400,  0.010) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_autoi( 0.308,  0.000) },
                { FOOTAG_LEADS,         .data.i = 6 },
                { FOOTAG_LEAD_DIAM,     .data.t = footol_autoi( 0.040,  0.010) },
                { FOOTAG_LEAD_DIST,     .data.t = footol_autoi( 0.375,  0.010) },
                { FOOTAG_PITCH,         .data.d = footol_inchtomm(0.140) },
                { FOOTAG_PINNAMES,      .data.sl = (const char *const []) {
                                "1", "2", "3", "4", "-", "5", NULL
                        }
                },

                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}
{
        struct footag_item tis[FOOTAG_NUM] = {
                { FOOTAG_FOOTYPE,       .data.i = FOOTYPE_SIDECONCAVE },
                { NAME_PREFIX,          .data.s = "OSCSC" },
                { NAME_SUFFIX,          .data.s = "ASFL1" },
                { FOOTAG_BODY_LENGTH,   .data.t = footol_auto( 5.00,  0.20) },
                { FOOTAG_BODY_WIDTH,    .data.t = footol_auto( 3.20,  0.20) },
                { FOOTAG_BODY_HEIGHT,   .data.t = footol_auto( 1.30,  0.00) },
                { FOOTAG_LEADS,         .data.i = 4 },
                { FOOTAG_PITCH,         .data.d = 2.50 },
                { FOOTAG_LEAD_LENGTH,   .data.t = footol_auto( 1.00,  0.15) },
                { FOOTAG_LEAD_WIDTH,    .data.t = footol_auto( 1.20,  0.10) },
                { FOOTAG_END, {0}},
        };
        dosimple(tis, OUTDIR);
}

        return 0;
}

