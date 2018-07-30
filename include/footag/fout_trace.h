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

#ifndef FOOTAG_FOUT_TRACE_H
#define FOOTAG_FOUT_TRACE_H

/*
 * Debug output module
 *
 * This output generator prints information on how it is invoked. It can be
 * used as a starting point for a new output generator.
 */

#include <footag/fout.h>

/*
 * filename: a file for log output
 */
struct fout_ctx *fout_trace_open(
        const char *filename
);

#endif

