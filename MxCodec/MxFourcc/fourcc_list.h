/*****************************************************************************
 * fourcc.c: fourcc helpers functions
 *****************************************************************************
 * Copyright © 2009-2011 Laurent Aimar
 *
 * Authors: Laurent Aimar <fenrir@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

typedef struct
{
    bool klass;
    unsigned char fourcc[5];
    const char *description;
} staticentry_t;

struct entry
{
    char fourcc[4];
    char alias[4];
    const char *desc;
};


extern const staticentry_t p_list_video[];
extern const staticentry_t p_list_audio[];
extern staticentry_t p_list_spu[];
