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

#include "footson.h"

enum {
        FOOTSON_IND = 4
};

int footson_init(struct footson_ctx *ctx)
{
        ctx->stream = NULL;
        ctx->level = 0;
        ctx->first = 1;
        return FOOTSON_OK;
}

static int ind(struct footson_ctx *ctx)
{
        for (int i = 0; i < ctx->level * FOOTSON_IND; i++) {
                int ret;
                ret = fputc(' ', ctx->stream);
                if (ret == EOF) {
                        return FOOTSON_IO;
                }
        }
        return FOOTSON_OK;
}

int footson_push(struct footson_ctx *ctx, const char *str)
{
        int ret;
        ret = fprintf(ctx->stream, "%s", str);
        if (ret < 0) {
                return FOOTSON_IO;
        }
        ctx->level++;
        ctx->first |= (1 << ctx->level);
        return FOOTSON_OK;
}

int footson_pop(struct footson_ctx *ctx, const char *str)
{
        int ret;
        ctx->first &= ~(1 << ctx->level);
        ctx->level--;
        ret = fprintf(ctx->stream, "\n");
        ret = ind(ctx);
        if (ret) { return ret; }
        ret = fprintf(ctx->stream, "%s", str);
        if (ret < 0) { return FOOTSON_IO; }
        return FOOTSON_OK;
}

int footson_openobj(struct footson_ctx *ctx)
{
        if (ctx->first & (1 << ctx->level)) {
                fprintf(ctx->stream, "\n");
                ctx->first &= ~(1 << ctx->level);
        } else {
                fprintf(ctx->stream, ",\n");
        }
        ind(ctx);
        footson_push(ctx, "{");
        return FOOTSON_OK;
}

int footson_closeobj(struct footson_ctx *ctx)
{
        footson_pop(ctx, "}");
        return FOOTSON_OK;
}

int footson_emptyobj(struct footson_ctx *ctx, const char *name)
{
        footson_addmember(ctx, name);
        fprintf(ctx->stream, "{}");
        return FOOTSON_OK;
}

int footson_addstring(struct footson_ctx *ctx, const char *str)
{
        if (ctx->first & (1 << ctx->level)) {
                fprintf(ctx->stream, "\n");
                ctx->first &= ~(1 << ctx->level);
        } else {
                fprintf(ctx->stream, ",\n");
        }
        ind(ctx);
        fprintf(ctx->stream, "\"%s\"", str);
        return FOOTSON_OK;
}

int footson_addmember(struct footson_ctx *ctx, const char *name)
{
        footson_addstring(ctx, name);
        fprintf(ctx->stream, ": ");
        return FOOTSON_OK;
}

int footson_dostring(struct footson_ctx *ctx, const char *name, const char *str)
{
        footson_addmember(ctx, name);
        fprintf(ctx->stream, "\"%s\"", str);
        return FOOTSON_OK;
}

int footson_dolong(struct footson_ctx *ctx, const char *name, long n)
{
        footson_addmember(ctx, name);
        fprintf(ctx->stream, "%ld", n);
        return FOOTSON_OK;
}

int footson_dobool(struct footson_ctx *ctx, const char *name, int val)
{
        footson_addmember(ctx, name);
        fprintf(ctx->stream, "%s", (const char *[]) { "false", "true" }[!!val]);
        return FOOTSON_OK;
}

