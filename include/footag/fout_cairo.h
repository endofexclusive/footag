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

#ifndef FOOTAG_FOUT_CAIRO_H
#define FOOTAG_FOUT_CAIRO_H

/*
 * Cairo output modules for SVG, PDF and PostScript
 */

#include <footag/fout.h>

/*
 * Width and height unit is millimeter. 1 SVG point is 1/72.0 inch.
 */
struct fout_ctx *fout_cairo_svg_open(
        const char *filename,
        double width,
        double height,
        double scale
);

struct fout_ctx *fout_cairo_pdf_open(
        const char *filename,
        double width,
        double height,
        double scale
);

struct fout_ctx *fout_cairo_ps_open(
        const char *filename,
        double width,
        double height,
        double scale
);

#endif

