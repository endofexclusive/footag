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

#ifndef FOOTAG_FOUT_HORIZON_H
#define FOOTAG_FOUT_HORIZON_H

/*
 * Horizon package output module
 */

#include <footag/fout.h>

/*
 * The parameter is a directory name where the output file will be written to
 * as $dirname/package.json. $dirname is created if not already existing.
 */
struct fout_ctx *fout_horizon_open(
        const char *dirname
);

#endif

