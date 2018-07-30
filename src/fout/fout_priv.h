#ifndef FOUT_PRIV_H
#define FOUT_PRIV_H

#include <footag/fout.h>
#include <footag/geom.h>
#include <footag/geom_types.h>

#ifndef UNUSED
 #define UNUSED(i) (void) (sizeof (i))
#endif

struct fout_ops {
        const char *desc;
        int (*fini)(struct fout_ctx *ctx);
        int (*setname)(struct fout_ctx *ctx, const char *name);
        /* called once before any shape processing */
        int (*pregen)(struct fout_ctx *ctx, const struct geom_shape *head);
        /* called once after all shape processing */
        int (*postgen)(struct fout_ctx *ctx, const struct geom_shape *head);
        /* called once before shapes on layer are processed with doshape() */
        int (*prelayer)(struct fout_ctx *ctx, int layer);
        /* called once after shapes on layer are processed with doshape() */
        int (*postlayer)(struct fout_ctx *ctx, int layer);
        /* called once for each shape */
        int (*doshape)(struct fout_ctx *fout, const struct geom_shape *shape);
};

struct fout_ctx {
        const struct fout_ops *ops;
};

int fout_setname_none(struct fout_ctx *ctx, const char *name);
int fout_gen_none(struct fout_ctx *ctx, const struct geom_shape *head);
int fout_layer_none(struct fout_ctx *ctx, int layer);

#endif

