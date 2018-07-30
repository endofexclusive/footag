#ifndef FUTIL_H
#define FUTIL_H

#include <footag/footag.h>

/* Our custom tags for convenience */
enum {
        NAME_PREFIX = FOOTAG_CUSTOM0,
        NAME_SUFFIX,
};

int dosimple(const struct footag_item *tis, const char *outdir);

#endif

