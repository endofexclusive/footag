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

#include "foolog.h"
#include <stdio.h>

static const char *const FOOLOG_LEVEL_TO_STR[FOOLOG_LEVEL_NUM] = {
        "FAIL",
        "ERROR",
        "WARNING",
        "INFO",
        "DEBUG",
};

void foolog_stderr(
        struct foolog_ctx *ctx,
        int level,
        const char *sub,
        const char *file,
        const char *func,
        const char *fmt,
        va_list args
)
{
        (void) ctx;
        fprintf(
                stderr,
                "%-7s %s: %s:%s(): ",
                FOOLOG_LEVEL_TO_STR[level],
                sub,
                file,
                func
        );
        vfprintf(stderr, fmt, args);
}

