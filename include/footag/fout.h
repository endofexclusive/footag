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

#ifndef FOOTAG_FOUT_H
#define FOOTAG_FOUT_H

enum {
        FOUT_OK,
        FOUT_FAIL,
};

struct geom_ctx;
struct fout_ctx;

int fout_close(
        struct fout_ctx *ctx
);

int fout_gen(
        struct fout_ctx *ctx,
        const struct geom_ctx *geom,
        const char *name
);

#endif

