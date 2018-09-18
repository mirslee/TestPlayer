/*****************************************************************************
 * vlc_es.h: Elementary stream formats descriptions
 *****************************************************************************
 * Copyright (C) 1999-2012 VLC authors and VideoLAN
 * $Id: c03fca394aeef77ad642defe241bd5a5ffe5d53d $
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
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

#ifndef MXES_H
#define MXES_H 1

/** ES Categories */
enum es_format_category_e
{
    UNKNOWN_ES = 0x00,
    VIDEO_ES,
    AUDIO_ES,
    SPU_ES,
    DATA_ES,
};

#endif
