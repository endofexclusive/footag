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

#ifndef FOOTAG_FOUT_KICAD5_H
#define FOOTAG_FOUT_KICAD5_H

/*
 * KiCad 5.0 footprint output module
 *
 * The output may be compatible with KiCad 4.0 if rounded rectangle pads are
 * not used.
 */

#include <footag/fout.h>

struct fout_ctx *fout_kicad5_open(
        const char *filename
);

#endif

