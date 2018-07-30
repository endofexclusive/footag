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

#ifndef FOOTAG_GEOM_TYPES_H
#define FOOTAG_GEOM_TYPES_H

/*
 * geom does not give specific meaning to layers or pad types. Define some here
 * for our convenience.
 */

enum {
        GEOM_LAYER_CYARD,
        GEOM_LAYER_COPPER,
        GEOM_LAYER_SILK,
        GEOM_LAYER_ASSY,
        GEOM_LAYER_NUM,
        GEOM_LAYER_CUSTOM = 0x100,
};

enum {
        GEOM_PAD_RECT,
        GEOM_PAD_RRECT,
        GEOM_PAD_CIRC,
        GEOM_PAD_TRECT,
        GEOM_PAD_TCIRC,
        GEOM_PAD_NUM,
        GEOM_PAD_CUSTOM = 0x100,
};

#endif

