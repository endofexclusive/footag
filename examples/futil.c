#include <errno.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <footag/foot.h>
#include <footag/geom.h>
#include <footag/fout_trace.h>
#include <footag/fout_kicad5.h>
#include <footag/fout_cairo.h>
#include <footag/fout_horizon.h>
#include "futil.h"

static const char *gettagstr(const struct footag_item *tis, int tag)
{
        const struct footag_item *ti;

        ti = footag_find(tis, tag);
        if (ti) {
                return ti->data.s;
        }
        return NULL;
}

static int docommon(struct fout_ctx *fout, struct geom_ctx *geom, const char *name)
{
        int ret;
        ret = fout_gen(fout, geom, name);
        if (ret) {
                printf("ERROR: fout_gen() -> %d\n", ret);
                return 2;
        }
        return 0;
}

static char *allocname(const char *pre, const char *mid, const char *suf)
{
        size_t fnlen = strlen(pre) + strlen(mid) + strlen(suf) + 1;
        char *fn = calloc(fnlen, sizeof *fn);
        assert(fn);
        snprintf(fn, fnlen, "%s%s%s", pre, mid, suf);
        fn[fnlen-1] = '\0';
        return fn;
}

static int dotrace(struct geom_ctx *geom, const char *dir, const char *name)
{
        int ret = 0;
        struct fout_ctx *fout = NULL;
        char *hfn = NULL;

        hfn = allocname(dir, name, ".trace");
        fout = fout_trace_open(hfn);
        if (!fout) {
                printf("ERROR: fout_trace_open(\"%s\") -> %p\n", name, (void *) fout);
                free(hfn);
                return 1;
        }
        ret = docommon(fout, geom, name);
        fout_close(fout);
        free(hfn);
        return ret;
}

static int dokicad5(struct geom_ctx *geom, const char *dir, const char *name)
{
        int ret = 0;
        struct fout_ctx *fout = NULL;
        char *hfn = NULL;

        hfn = allocname(dir, name, ".kicad_mod");
        fout = fout_kicad5_open(hfn);
        if (!fout) {
                printf("ERROR: fout_kicad5_open(\"%s\") -> %p\n", name, (void *) fout);
                free(hfn);
                return 1;
        }
        ret = docommon(fout, geom, name);
        fout_close(fout);
        free(hfn);
        return ret;
}

static int docairo(struct geom_ctx *geom, const char *dir, const char *name, double w, double h)
{
        int ret = 0;
        double scale;
        struct fout_ctx *fout = NULL;
        char *hfn = NULL;

        h = fmax(w, h) * 1.1;
        w = h * FOOT_GOLDEN_RATIO;
        scale = 160.0 / h;

        hfn = allocname(dir, name, ".svg");
        fout = fout_cairo_svg_open(hfn, w, h, scale);
        if (!fout) {
                printf("ERROR: fout_cairo_svg_open(\"%s\") -> %p\n", name, (void *) fout);
                free(hfn);
                return 1;
        }
        ret = docommon(fout, geom, name);
        fout_close(fout);
        free(hfn);
        return ret;
}

static int dohorizon(struct geom_ctx *geom, const char *dir, const char *name)
{
        int ret = 0;
        struct fout_ctx *fout = NULL;
        char *hfn = NULL;

        hfn = allocname(dir, name, "");
        fout = fout_horizon_open(hfn);
        if (!fout) {
                printf("ERROR: fout_horizon_open(\"%s\") -> %p\n", name, (void *) fout);
                free(hfn);
                return 1;
        }
        ret = docommon(fout, geom, name);
        fout_close(fout);
        free(hfn);
        return ret;
}

static int mkthedir(const char *dirname)
{
        int ret;
        ret = mkdir(dirname, S_IRWXU | S_IRWXG | S_IRWXO);
        if (ret == 0 || errno == EEXIST) {
                return 0;
        }
        return 1;
}

int dosimple(const struct footag_item *tis, const char *outdir)
{
        int ret = 0;
        struct foot_ctx *foot = NULL;
        struct geom_ctx *geom = NULL;
        char *name = NULL;
        double w;
        double h;
        const char *pre = gettagstr(tis, NAME_PREFIX);
        const char *suf = gettagstr(tis, NAME_SUFFIX);

        ret = mkthedir(outdir);
        if (ret) {
                printf("mkthedir: %s: %s\n", outdir, strerror(errno));
                goto out;
        }

        foot = foot_open(tis);
        if (!foot) {
                printf("ERROR: foot_open() -> %p\n", (void *) foot);
                goto out;
        }

        ret = foot_calc(foot);
        if (ret) {
                printf("ERROR: foot_calc() -> %d\n", ret);
                goto out;
        }

        geom = geom_open();
        if (!geom) {
                printf("ERROR: geom_open -> %p\n", (void *) geom);
                goto out;
        }

        ret = foot_gen(foot, geom);
        if (ret) {
                printf("ERROR: foot_gen() -> %d\n", ret);
                goto out;
        }

        name = foot_name(foot, pre, suf);
        if (!name) {
                printf("ERROR: foot_name() -> %d\n", ret);
                goto out;
        }
        printf("* generating %s\n", name);

        foot_getbounds(foot, &w, &h);

        dotrace(geom, outdir, name);
        dokicad5(geom, outdir, name);
        docairo(geom, outdir, name, w, h);
        dohorizon(geom, outdir, name);

out:
        free(name);
        if (geom) {
                geom_close(geom);
        }
        if (foot) {
                foot_close(foot);
        }

        return ret;
}

