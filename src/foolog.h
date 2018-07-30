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

#ifndef FOOLOG_H
#define FOOLOG_H

#include <stdarg.h>

struct foolog_ctx;

enum {
        FOOLOG_LEVEL_FAIL,
        FOOLOG_LEVEL_ERROR,
        FOOLOG_LEVEL_WARNING,
        FOOLOG_LEVEL_INFO,
        FOOLOG_LEVEL_DEBUG,
        FOOLOG_LEVEL_NUM,
};

enum {
        FOOLOG_FAIL     = (1 << FOOLOG_LEVEL_FAIL),
        FOOLOG_ERROR    = (1 << FOOLOG_LEVEL_ERROR),
        FOOLOG_WARNING  = (1 << FOOLOG_LEVEL_WARNING),
        FOOLOG_INFO     = (1 << FOOLOG_LEVEL_INFO),
        FOOLOG_DEBUG    = (1 << FOOLOG_LEVEL_DEBUG),
        FOOLOG_NONE     = 0,
        FOOLOG_ALL      = (1 << FOOLOG_LEVEL_NUM) - 1,
        FOOLOG_DEFAULT  = FOOLOG_ALL & ~FOOLOG_DEBUG
};

typedef void (foolog_func)(
        struct foolog_ctx *ctx,
        int level,
        const char *sub,
        const char *file,
        const char *func,
        const char *fmt,
        va_list args
);

foolog_func foolog_stderr;
struct foolog_ctx {
        /* Set by user at run-time to enable or disable log levels */
        int mask;
        /* A user specified function which handles log messages */
        foolog_func *func;
};

static inline void FOOLOG_EMITTER(
        struct foolog_ctx *ctx,
        int level,
        const char *sub,
        const char *file,
        const char *func,
        const char *fmt,
        ...
)
{
        va_list args;
        va_start(args, fmt);
        ctx->func(ctx, level, sub, file, func, fmt, args);
        va_end(args);
}

#ifndef FOOLOG_MASK
 #define FOOLOG_MASK FOOLOG_DEFAULT
#endif

#ifndef FOOLOG_SUBSYS
 #define FOOLOG_SUBSYS "UNKNOWN"
#endif

#define foolog_msg(level, ctx, ...) \
        if (FOOLOG_MASK & (1 << level)) { \
                FOOLOG_EMITTER( \
                        ctx, \
                        level, \
                        FOOLOG_SUBSYS, \
                        __FILE__, \
                        __func__, \
                        __VA_ARGS__ \
                ); \
        }

#endif

