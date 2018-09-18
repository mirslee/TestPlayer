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

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include "stdafx.h"
#include "MxConfig.h"
#include "MxCommon.h"
#include "MxFourcc.h"

#include "MxEs.h"
#include <assert.h>

#include "fourcc_tables.h"

static int fourcc_cmp(const void *key, const void *ent)
{
    return memcmp(key, ent, 4);
}

static MxFourcc Lookup(MxFourcc fourcc, const char **/*restrict*/ dsc,
                           const struct fourcc_mapping *mapv, size_t mapc,
                           const struct fourcc_desc *dscv, size_t dscc)
{
    const struct fourcc_mapping *mapping;
    const struct fourcc_desc *desc;

    mapping = (const struct fourcc_mapping*)bsearch(&fourcc, mapv, mapc, sizeof (*mapv), fourcc_cmp);
    if (mapping != NULL)
    {
        if (dsc != NULL)
        {
            desc = (const struct fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof (*dscv), fourcc_cmp);
            if (desc != NULL)
            {
                *dsc = desc->desc;
                return mapping->fourcc;
            }
        }
        fourcc = mapping->fourcc;
    }

    desc = (const struct fourcc_desc*)bsearch(&fourcc, dscv, dscc, sizeof (*dscv), fourcc_cmp);
    if (desc == NULL)
        return 0; /* Unknown FourCC */
    if (dsc != NULL)
        *dsc = desc->desc;
    return fourcc; /* Known FourCC (has a description) */
}

static MxFourcc LookupVideo(MxFourcc fourcc, const char **/*restrict*/ dsc)
{
    return Lookup(fourcc, dsc, mapping_video,
                  sizeof (mapping_video) / sizeof (mapping_video[0]),
                  desc_video, sizeof (desc_video) / sizeof (desc_video[0]));
}

static MxFourcc LookupAudio(MxFourcc fourcc, const char **/*restrict*/ dsc)
{
    return Lookup(fourcc, dsc, mapping_audio,
                  sizeof (mapping_audio) / sizeof (mapping_audio[0]),
                  desc_audio, sizeof (desc_audio) / sizeof (desc_audio[0]));
}

static MxFourcc LookupSpu(MxFourcc fourcc, const char **/*restrict*/ dsc)
{
    return Lookup(fourcc, dsc, mapping_spu,
                  sizeof (mapping_spu) / sizeof (mapping_spu[0]),
                  desc_spu, sizeof (desc_spu) / sizeof (desc_spu[0]));
}

static MxFourcc LookupCat(MxFourcc fourcc, const char **/*restrict*/ dsc,
                              int cat)
{
    switch (cat)
    {
        case VIDEO_ES:
            return LookupVideo(fourcc, dsc);
        case AUDIO_ES:
            return LookupAudio(fourcc, dsc);
        case SPU_ES:
            return LookupSpu(fourcc, dsc);
    }

    MxFourcc ret = LookupVideo(fourcc, dsc);
    if (!ret)
        ret = LookupAudio(fourcc, dsc);
    if (!ret)
        ret = LookupSpu(fourcc, dsc);
    return ret;
}

MxFourcc mxFourccGetCodec(int cat, MxFourcc fourcc)
{
    MxFourcc codec = LookupCat(fourcc, NULL, cat);
    return codec ? codec : fourcc;
}

MxFourcc mxFourccGetCodecFromString( int i_cat, const char *psz_fourcc )
{
    if( !psz_fourcc || strlen(psz_fourcc) != 4 )
        return 0;
    return mxFourccGetCodec( i_cat,
                                MX_FOURCC( psz_fourcc[0], psz_fourcc[1],
                                            psz_fourcc[2], psz_fourcc[3] ) );
}

MxFourcc mxFourccGetCodecAudio( MxFourcc i_fourcc, int i_bits )
{
    const int i_bytes = ( i_bits + 7 ) / 8;

    if( i_fourcc == MX_FOURCC( 'a', 'f', 'l', 't' ) )
    {
        switch( i_bytes )
        {
        case 4:
            return MX_CODEC_FL32;
        case 8:
            return MX_CODEC_FL64;
        default:
            return 0;
        }
    }
    else if( i_fourcc == MX_FOURCC( 'a', 'r', 'a', 'w' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return MX_CODEC_U8;
        case 2:
            return MX_CODEC_S16L;
        case 3:
            return MX_CODEC_S24L;
        case 4:
            return MX_CODEC_S32L;
        default:
            return 0;
        }
    }
    else if( i_fourcc == MX_FOURCC( 't', 'w', 'o', 's' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return MX_CODEC_S8;
        case 2:
            return MX_CODEC_S16B;
        case 3:
            return MX_CODEC_S24B;
        case 4:
            return MX_CODEC_S32B;
        default:
            return 0;
        }
    }
    else if( i_fourcc == MX_FOURCC( 's', 'o', 'w', 't' ) )
    {
        switch( i_bytes )
        {
        case 1:
            return MX_CODEC_S8;
        case 2:
            return MX_CODEC_S16L;
        case 3:
            return MX_CODEC_S24L;
        case 4:
            return MX_CODEC_S32L;
        default:
            return 0;
        }
    }
    else
    {
        return mxFourccGetCodec( AUDIO_ES, i_fourcc );
    }
}

const char *mxFourccGetDescription(int cat, MxFourcc fourcc)
{
    const char *ret;

    return LookupCat(fourcc, &ret, cat) ? ret : "";
}


/* */
#define MX_CODEC_YUV_PLANAR_410 \
    MX_CODEC_I410, MX_CODEC_YV9

#define MX_CODEC_YUV_PLANAR_420 \
    MX_CODEC_I420, MX_CODEC_YV12, MX_CODEC_J420

#define MX_CODEC_YUV_SEMIPLANAR_420 \
    MX_CODEC_NV12, MX_CODEC_NV21

#define MX_CODEC_YUV_PLANAR_420_16 \
    MX_CODEC_I420_16L, MX_CODEC_I420_16B, MX_CODEC_I420_12L, MX_CODEC_I420_12B, MX_CODEC_I420_10L, MX_CODEC_I420_10B, MX_CODEC_I420_9L, MX_CODEC_I420_9B

#define MX_CODEC_YUV_SEMIPLANAR_420_16 \
    MX_CODEC_P010

#define MX_CODEC_YUV_PLANAR_422 \
    MX_CODEC_I422, MX_CODEC_J422

#define MX_CODEC_YUV_SEMIPLANAR_422 \
    MX_CODEC_NV16, MX_CODEC_NV61

#define MX_CODEC_YUV_PLANAR_422_16 \
    MX_CODEC_I422_12L, MX_CODEC_I422_12B, MX_CODEC_I422_10L, MX_CODEC_I422_10B, MX_CODEC_I422_9L, MX_CODEC_I422_9B

#define MX_CODEC_YUV_PLANAR_440 \
    MX_CODEC_I440, MX_CODEC_J440

#define MX_CODEC_YUV_PLANAR_444 \
    MX_CODEC_I444, MX_CODEC_J444

#define MX_CODEC_YUV_SEMIPLANAR_444 \
    MX_CODEC_NV24, MX_CODEC_NV42

#define MX_CODEC_YUV_PLANAR_444_16 \
    MX_CODEC_I444_10L, MX_CODEC_I444_10B, MX_CODEC_I444_9L, MX_CODEC_I444_9B, \
    MX_CODEC_I444_16L, MX_CODEC_I444_16B, MX_CODEC_I444_12L, MX_CODEC_I444_12B

#define MX_CODEC_YUV_PACKED \
    MX_CODEC_YUYV, MX_CODEC_YVYU, \
    MX_CODEC_UYVY, MX_CODEC_VYUY

#define MX_CODEC_FALLBACK_420 \
    MX_CODEC_YUV_PLANAR_422, MX_CODEC_YUV_PACKED, \
    MX_CODEC_YUV_PLANAR_444, MX_CODEC_YUV_PLANAR_440, \
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211

static const MxFourcc p_I420_fallback[] = {
    MX_CODEC_I420, MX_CODEC_YV12, MX_CODEC_J420, MX_CODEC_FALLBACK_420, 0
};
static const MxFourcc p_J420_fallback[] = {
    MX_CODEC_J420, MX_CODEC_I420, MX_CODEC_YV12, MX_CODEC_FALLBACK_420, 0
};
static const MxFourcc p_YV12_fallback[] = {
    MX_CODEC_YV12, MX_CODEC_I420, MX_CODEC_J420, MX_CODEC_FALLBACK_420, 0
};
static const MxFourcc p_NV12_fallback[] = {
    MX_CODEC_NV12, MX_CODEC_I420, MX_CODEC_J420, MX_CODEC_FALLBACK_420, 0
};

#define MX_CODEC_FALLBACK_420_16 \
    MX_CODEC_I420, MX_CODEC_YV12, MX_CODEC_J420, MX_CODEC_FALLBACK_420

static const MxFourcc p_I420_9L_fallback[] = {
    MX_CODEC_I420_9L, MX_CODEC_I420_9B, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_9B_fallback[] = {
    MX_CODEC_I420_9B, MX_CODEC_I420_9L, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_10L_fallback[] = {
    MX_CODEC_I420_10L, MX_CODEC_I420_10B, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_10B_fallback[] = {
    MX_CODEC_I420_10B, MX_CODEC_I420_10L, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_12L_fallback[] = {
    MX_CODEC_I420_12L, MX_CODEC_I420_12B, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_12B_fallback[] = {
    MX_CODEC_I420_12B, MX_CODEC_I420_12L, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_16L_fallback[] = {
    MX_CODEC_I420_16L, MX_CODEC_I420_16B, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_I420_16B_fallback[] = {
    MX_CODEC_I420_16B, MX_CODEC_I420_16L, MX_CODEC_FALLBACK_420_16, 0
};
static const MxFourcc p_P010_fallback[] = {
    MX_CODEC_P010, MX_CODEC_FALLBACK_420_16, 0
};


#define MX_CODEC_FALLBACK_422 \
    MX_CODEC_YUV_PACKED, MX_CODEC_YUV_PLANAR_420, \
    MX_CODEC_YUV_PLANAR_444, MX_CODEC_YUV_PLANAR_440, \
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211

static const MxFourcc p_I422_fallback[] = {
    MX_CODEC_I422, MX_CODEC_J422, MX_CODEC_FALLBACK_422, 0
};
static const MxFourcc p_J422_fallback[] = {
    MX_CODEC_J422, MX_CODEC_I422, MX_CODEC_FALLBACK_422, 0
};

#define MX_CODEC_FALLBACK_422_16 \
    MX_CODEC_I422, MX_CODEC_J422, MX_CODEC_FALLBACK_422

static const MxFourcc p_I422_9L_fallback[] = {
    MX_CODEC_I422_9L, MX_CODEC_I422_9B, MX_CODEC_FALLBACK_422_16, 0
};
static const MxFourcc p_I422_9B_fallback[] = {
    MX_CODEC_I422_9B, MX_CODEC_I422_9L, MX_CODEC_FALLBACK_422_16, 0
};
static const MxFourcc p_I422_10L_fallback[] = {
    MX_CODEC_I422_10L, MX_CODEC_I422_10B, MX_CODEC_FALLBACK_422_16, 0
};
static const MxFourcc p_I422_10B_fallback[] = {
    MX_CODEC_I422_10B, MX_CODEC_I422_10L, MX_CODEC_FALLBACK_422_16, 0
};
static const MxFourcc p_I422_12L_fallback[] = {
    MX_CODEC_I422_12L, MX_CODEC_I422_12B, MX_CODEC_FALLBACK_422_16, 0
};
static const MxFourcc p_I422_12B_fallback[] = {
    MX_CODEC_I422_12B, MX_CODEC_I422_12L, MX_CODEC_FALLBACK_422_16, 0
};

#define MX_CODEC_FALLBACK_444 \
    MX_CODEC_YUV_PLANAR_422, MX_CODEC_YUV_PACKED, \
    MX_CODEC_YUV_PLANAR_420, MX_CODEC_YUV_PLANAR_440, \
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211

static const MxFourcc p_I444_fallback[] = {
    MX_CODEC_I444, MX_CODEC_J444, MX_CODEC_FALLBACK_444, 0
};
static const MxFourcc p_J444_fallback[] = {
    MX_CODEC_J444, MX_CODEC_I444, MX_CODEC_FALLBACK_444, 0
};

#define MX_CODEC_FALLBACK_444_16 \
    MX_CODEC_I444, MX_CODEC_J444, MX_CODEC_FALLBACK_444

static const MxFourcc p_I444_9L_fallback[] = {
    MX_CODEC_I444_9L, MX_CODEC_I444_9B, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_9B_fallback[] = {
    MX_CODEC_I444_9B, MX_CODEC_I444_9L, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_10L_fallback[] = {
    MX_CODEC_I444_10L, MX_CODEC_I444_10B, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_10B_fallback[] = {
    MX_CODEC_I444_10B, MX_CODEC_I444_10L, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_12L_fallback[] = {
    MX_CODEC_I444_12L, MX_CODEC_I444_12B, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_12B_fallback[] = {
    MX_CODEC_I444_12B, MX_CODEC_I444_12L, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_16L_fallback[] = {
    MX_CODEC_I444_16L, MX_CODEC_I444_16B, MX_CODEC_FALLBACK_444_16, 0
};
static const MxFourcc p_I444_16B_fallback[] = {
    MX_CODEC_I444_16B, MX_CODEC_I444_16L, MX_CODEC_FALLBACK_444_16, 0
};


/* Fallbacks for cvpx */
static const MxFourcc p_CVPX_VIDEO_NV12_fallback[] = {
    MX_CODEC_CVPX_NV12, MX_CODEC_NV12, MX_CODEC_I420, 0,
};
static const MxFourcc p_CVPX_VIDEO_UYVY_fallback[] = {
    MX_CODEC_CVPX_UYVY, MX_CODEC_UYVY, 0,
};
static const MxFourcc p_CVPX_VIDEO_I420_fallback[] = {
    MX_CODEC_CVPX_I420, MX_CODEC_I420, 0,
};
static const MxFourcc p_CVPX_VIDEO_BGRA_fallback[] = {
    MX_CODEC_CVPX_BGRA, MX_CODEC_BGRA, 0,
};
static const MxFourcc p_CVPX_VIDEO_P010_fallback[] = {
    MX_CODEC_CVPX_P010, MX_CODEC_P010, MX_CODEC_I420_10L, 0
};

static const MxFourcc p_VAAPI_420_fallback[] = {
    MX_CODEC_VAAPI_420, MX_CODEC_I420, 0,
};

static const MxFourcc p_VAAPI_420_10BPP_fallback[] = {
    MX_CODEC_VAAPI_420_10BPP, MX_CODEC_P010, MX_CODEC_I420_10L, 0,
};

static const MxFourcc p_D3D9_OPAQUE_fallback[] = {
    MX_CODEC_D3D9_OPAQUE, MX_CODEC_I420, 0,
};

static const MxFourcc p_D3D9_OPAQUE_10B_fallback[] = {
    MX_CODEC_D3D9_OPAQUE_10B, MX_CODEC_P010, MX_CODEC_I420_10L, 0,
};

static const MxFourcc p_D3D11_OPAQUE_fallback[] = {
    MX_CODEC_D3D11_OPAQUE, MX_CODEC_NV12, 0,
};

static const MxFourcc p_D3D11_OPAQUE_10B_fallback[] = {
    MX_CODEC_D3D11_OPAQUE_10B, MX_CODEC_P010, MX_CODEC_I420_10L, 0,
};

static const MxFourcc p_I440_fallback[] = {
    MX_CODEC_I440,
    MX_CODEC_YUV_PLANAR_420,
    MX_CODEC_YUV_PLANAR_422,
    MX_CODEC_YUV_PLANAR_444,
    MX_CODEC_YUV_PACKED,
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211, 0
};

#define MX_CODEC_FALLBACK_PACKED \
    MX_CODEC_YUV_PLANAR_422, MX_CODEC_YUV_PLANAR_420, \
    MX_CODEC_YUV_PLANAR_444, MX_CODEC_YUV_PLANAR_440, \
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211

static const MxFourcc p_YUYV_fallback[] = {
    MX_CODEC_YUYV,
    MX_CODEC_YVYU,
    MX_CODEC_UYVY,
    MX_CODEC_VYUY,
    MX_CODEC_FALLBACK_PACKED, 0
};
static const MxFourcc p_YVYU_fallback[] = {
    MX_CODEC_YVYU,
    MX_CODEC_YUYV,
    MX_CODEC_UYVY,
    MX_CODEC_VYUY,
    MX_CODEC_FALLBACK_PACKED, 0
};
static const MxFourcc p_UYVY_fallback[] = {
    MX_CODEC_UYVY,
    MX_CODEC_VYUY,
    MX_CODEC_YUYV,
    MX_CODEC_YVYU,
    MX_CODEC_FALLBACK_PACKED, 0
};
static const MxFourcc p_VYUY_fallback[] = {
    MX_CODEC_VYUY,
    MX_CODEC_UYVY,
    MX_CODEC_YUYV,
    MX_CODEC_YVYU,
    MX_CODEC_FALLBACK_PACKED, 0
};

static const MxFourcc *pp_YUV_fallback[] = {
    p_YV12_fallback,
    p_I420_fallback,
    p_I420_9L_fallback,
    p_I420_9B_fallback,
    p_I420_10L_fallback,
    p_I420_10B_fallback,
    p_I420_12L_fallback,
    p_I420_12B_fallback,
    p_I420_16L_fallback,
    p_I420_16B_fallback,
    p_J420_fallback,
    p_I422_fallback,
    p_I422_9L_fallback,
    p_I422_9B_fallback,
    p_I422_10L_fallback,
    p_I422_10B_fallback,
    p_I422_12L_fallback,
    p_I422_12B_fallback,
    p_J422_fallback,
    p_I444_fallback,
    p_J444_fallback,
    p_I444_9L_fallback,
    p_I444_9B_fallback,
    p_I444_10L_fallback,
    p_I444_10B_fallback,
    p_I444_12L_fallback,
    p_I444_12B_fallback,
    p_I444_16L_fallback,
    p_I444_16B_fallback,
    p_I440_fallback,
    p_YUYV_fallback,
    p_YVYU_fallback,
    p_UYVY_fallback,
    p_VYUY_fallback,
    p_NV12_fallback,
    p_P010_fallback,
    p_CVPX_VIDEO_NV12_fallback,
    p_CVPX_VIDEO_UYVY_fallback,
    p_CVPX_VIDEO_I420_fallback,
    p_CVPX_VIDEO_P010_fallback,
    p_VAAPI_420_fallback,
    p_VAAPI_420_10BPP_fallback,
    p_D3D9_OPAQUE_fallback,
    p_D3D9_OPAQUE_10B_fallback,
    p_D3D11_OPAQUE_fallback,
    p_D3D11_OPAQUE_10B_fallback,
    NULL,
};

static const MxFourcc p_list_YUV[] = {
    MX_CODEC_YUV_PLANAR_420,
    MX_CODEC_YUV_SEMIPLANAR_420,
    MX_CODEC_YUV_PLANAR_422,
    MX_CODEC_YUV_SEMIPLANAR_422,
    MX_CODEC_YUV_PLANAR_440,
    MX_CODEC_YUV_PLANAR_444,
    MX_CODEC_YUV_SEMIPLANAR_444,
    MX_CODEC_YUV_PACKED,
    MX_CODEC_I411, MX_CODEC_YUV_PLANAR_410, MX_CODEC_Y211,
    MX_CODEC_YUV_PLANAR_420_16,
    MX_CODEC_YUV_SEMIPLANAR_420_16,
    MX_CODEC_YUV_PLANAR_422_16,
    MX_CODEC_YUV_PLANAR_444_16,
    MX_CODEC_VDPAU_VIDEO_420,
    MX_CODEC_VDPAU_VIDEO_422,
    MX_CODEC_VDPAU_VIDEO_444,
    MX_CODEC_CVPX_NV12,
    MX_CODEC_CVPX_UYVY,
    MX_CODEC_CVPX_I420,
    MX_CODEC_CVPX_P010,
    MX_CODEC_VAAPI_420,
    MX_CODEC_VAAPI_420_10BPP,
    MX_CODEC_D3D9_OPAQUE,
    MX_CODEC_D3D9_OPAQUE_10B,
    MX_CODEC_D3D11_OPAQUE,
    MX_CODEC_D3D11_OPAQUE_10B,
    0,
};

/* */
static const MxFourcc p_RGB32_fallback[] = {
    MX_CODEC_RGB32,
    MX_CODEC_RGB24,
    MX_CODEC_RGB16,
    MX_CODEC_RGB15,
    MX_CODEC_RGB8,
    0,
};
static const MxFourcc p_RGB24_fallback[] = {
    MX_CODEC_RGB24,
    MX_CODEC_RGB32,
    MX_CODEC_RGB16,
    MX_CODEC_RGB15,
    MX_CODEC_RGB8,
    0,
};
static const MxFourcc p_RGB16_fallback[] = {
    MX_CODEC_RGB16,
    MX_CODEC_RGB24,
    MX_CODEC_RGB32,
    MX_CODEC_RGB15,
    MX_CODEC_RGB8,
    0,
};
static const MxFourcc p_RGB15_fallback[] = {
    MX_CODEC_RGB15,
    MX_CODEC_RGB16,
    MX_CODEC_RGB24,
    MX_CODEC_RGB32,
    MX_CODEC_RGB8,
    0,
};
static const MxFourcc p_RGB8_fallback[] = {
    MX_CODEC_RGB8,
    MX_CODEC_RGB15,
    MX_CODEC_RGB16,
    MX_CODEC_RGB24,
    MX_CODEC_RGB32,
    0,
};
static const MxFourcc *pp_RGB_fallback[] = {
    p_RGB32_fallback,
    p_RGB24_fallback,
    p_RGB16_fallback,
    p_RGB15_fallback,
    p_RGB8_fallback,
    p_CVPX_VIDEO_BGRA_fallback,

    NULL,
};


/* */
static const MxFourcc *GetFallback( MxFourcc i_fourcc,
                                        const MxFourcc *pp_fallback[],
                                        const MxFourcc p_list[] )
{
    for( unsigned i = 0; pp_fallback[i]; i++ )
    {
        if( pp_fallback[i][0] == i_fourcc )
            return pp_fallback[i];
    }
    return p_list;
}

const MxFourcc *mxFourccGetYUVFallback( MxFourcc i_fourcc )
{
    return GetFallback( i_fourcc, pp_YUV_fallback, p_list_YUV );
}
const MxFourcc *mxFourccGetRGBFallback( MxFourcc i_fourcc )
{
    return GetFallback( i_fourcc, pp_RGB_fallback, p_RGB32_fallback );
}

bool mxFourccAreUVPlanesSwapped( MxFourcc a, MxFourcc b )
{
    static const MxFourcc pp_swapped[][4] = {
        { MX_CODEC_YV12, MX_CODEC_I420, MX_CODEC_J420, 0 },
        { MX_CODEC_YV9,  MX_CODEC_I410, 0 },
        { 0 }
    };

    for( int i = 0; pp_swapped[i][0]; i++ )
    {
        if( pp_swapped[i][0] == b )
        {
            MxFourcc t = a;
            a = b;
            b = t;
        }
        if( pp_swapped[i][0] != a )
            continue;
        for( int j = 1; pp_swapped[i][j]; j++ )
        {
            if( pp_swapped[i][j] == b )
                return true;
        }
    }
    return false;
}

bool mxFourccIsYUV(MxFourcc fcc)
{
    for( unsigned i = 0; p_list_YUV[i]; i++ )
    {
        if( p_list_YUV[i] == fcc )
            return true;
    }
    return false;
}

#define PLANAR(n, w_den, h_den, size, bits) \
    { .plane_count = n, \
      .p = { {.w = {1,    1}, .h = {1,    1}}, \
             {.w = {1,w_den}, .h = {1,h_den}}, \
             {.w = {1,w_den}, .h = {1,h_den}}, \
             {.w = {1,    1}, .h = {1,    1}} }, \
      .pixel_size = size, \
      .pixel_bits = bits }

#define PLANAR_8(n, w_den, h_den)        PLANAR(n, w_den, h_den, 1, 8)
#define PLANAR_16(n, w_den, h_den, bits) PLANAR(n, w_den, h_den, 2, bits)

#define SEMIPLANAR(w_den, h_den, size, bits) \
    { .plane_count = 2, \
      .p = { {.w = {1,    1}, .h = {1,    1}}, \
             {.w = {2,w_den}, .h = {1,h_den}} }, \
      .pixel_size = size, \
      .pixel_bits = bits }

#define PACKED_FMT(size, bits) \
    { .plane_count = 1, \
      .p = { {.w = {1,1}, .h = {1,1}} }, \
      .pixel_size = size, \
      .pixel_bits = bits }

/* Zero planes for hardware picture handles. Cannot be manipulated directly. */
#define FAKE_FMT() \
    { .plane_count = 0, \
      .p = { {.w = {1,1}, .h = {1,1}} }, \
      .pixel_size = 0, \
      .pixel_bits = 0 }

static const struct
{
    MxFourcc             p_fourcc[4];
    MxChromaDescription description;
} p_list_chroma_description[] = {
    { { MX_CODEC_I411 },                      PLANAR_8(3, 4, 1) },
    { { MX_CODEC_YUV_PLANAR_410 },            PLANAR_8(3, 4, 4) },
    { { MX_CODEC_YUV_PLANAR_420 },            PLANAR_8(3, 2, 2) },
    { { MX_CODEC_NV12, MX_CODEC_NV21 },      SEMIPLANAR(2, 2, 1, 8) },
    { { MX_CODEC_YUV_PLANAR_422 },            PLANAR_8(3, 2, 1) },
    { { MX_CODEC_NV16, MX_CODEC_NV61 },      PLANAR_8(2, 1, 1) },
    { { MX_CODEC_YUV_PLANAR_440 },            PLANAR_8(3, 1, 2) },
    { { MX_CODEC_YUV_PLANAR_444 },            PLANAR_8(3, 1, 1) },
    { { MX_CODEC_YUVA },                      PLANAR_8(4, 1, 1) },
    { { MX_CODEC_YUV420A },                   PLANAR_8(4, 2, 2) },
    { { MX_CODEC_YUV422A },                   PLANAR_8(4, 2, 1) },

    { { MX_CODEC_GBR_PLANAR },                PLANAR_8(3, 1, 1) },
    { { MX_CODEC_GBR_PLANAR_9L,
        MX_CODEC_GBR_PLANAR_9B },             PLANAR_16(3, 1, 1, 9) },
    { { MX_CODEC_GBR_PLANAR_10L,
        MX_CODEC_GBR_PLANAR_10B },            PLANAR_16(3, 1, 1, 10) },

    { { MX_CODEC_I420_16L,
        MX_CODEC_I420_16B },                  PLANAR_16(3, 2, 2, 16) },
    { { MX_CODEC_I420_12L,
        MX_CODEC_I420_12B },                  PLANAR_16(3, 2, 2, 12) },
    { { MX_CODEC_I420_10L,
        MX_CODEC_I420_10B },                  PLANAR_16(3, 2, 2, 10) },
    { { MX_CODEC_I420_9L,
        MX_CODEC_I420_9B },                   PLANAR_16(3, 2, 2,  9) },
    { { MX_CODEC_I422_12L,
        MX_CODEC_I422_12B },                  PLANAR_16(3, 2, 1, 12) },
    { { MX_CODEC_I422_10L,
        MX_CODEC_I422_10B },                  PLANAR_16(3, 2, 1, 10) },
    { { MX_CODEC_I422_9L,
        MX_CODEC_I422_9B },                   PLANAR_16(3, 2, 1,  9) },
    { { MX_CODEC_I444_12L,
        MX_CODEC_I444_12B },                  PLANAR_16(3, 1, 1, 12) },
    { { MX_CODEC_I444_10L,
        MX_CODEC_I444_10B },                  PLANAR_16(3, 1, 1, 10) },
    { { MX_CODEC_I444_9L,
        MX_CODEC_I444_9B },                   PLANAR_16(3, 1, 1,  9) },
    { { MX_CODEC_I444_16L,
        MX_CODEC_I444_16B },                  PLANAR_16(3, 1, 1, 16) },
    { { MX_CODEC_YUVA_444_10L,
        MX_CODEC_YUVA_444_10B },              PLANAR_16(4, 1, 1, 10) },
    { { MX_CODEC_P010 },                      SEMIPLANAR(2, 2, 2, 10) },

    { { MX_CODEC_YUV_PACKED },                PACKED_FMT(2, 16) },
    { { MX_CODEC_RGB8, MX_CODEC_GREY,
        MX_CODEC_YUVP, MX_CODEC_RGBP },      PACKED_FMT(1, 8) },

    { { MX_CODEC_RGB15, 0 },                  PACKED_FMT(2, 15) },
    { { MX_CODEC_RGB12, 0 },                  PACKED_FMT(2, 12) },
    { { MX_CODEC_RGB16, 0 },                  PACKED_FMT(2, 16) },
    { { MX_CODEC_RGB24, 0 },                  PACKED_FMT(3, 24) },
    { { MX_CODEC_RGB32, 0 },                  PACKED_FMT(4, 24) },
    { { MX_CODEC_RGBA, MX_CODEC_ARGB,
        MX_CODEC_BGRA, },                     PACKED_FMT(4, 32) },

    { { MX_CODEC_Y211, 0 },                   { 1, { {{1,4}, {1,1}} }, 4, 32 } },
    { { MX_CODEC_XYZ12,  0 },                 PACKED_FMT(6, 48) },

    { { MX_CODEC_VDPAU_VIDEO_420, MX_CODEC_VDPAU_VIDEO_422,
        MX_CODEC_VDPAU_VIDEO_444, MX_CODEC_VDPAU_OUTPUT },
                                               FAKE_FMT() },
    { { MX_CODEC_ANDROID_OPAQUE, MX_CODEC_MMAL_OPAQUE,
        MX_CODEC_D3D9_OPAQUE,    MX_CODEC_D3D11_OPAQUE },
                                               FAKE_FMT() },
    { { MX_CODEC_D3D11_OPAQUE_10B, MX_CODEC_D3D9_OPAQUE_10B },
                                               FAKE_FMT() },

    { { MX_CODEC_CVPX_NV12, MX_CODEC_CVPX_UYVY,
        MX_CODEC_CVPX_I420, MX_CODEC_CVPX_BGRA },
                                               FAKE_FMT() },

    { { MX_CODEC_CVPX_P010, 0 },              FAKE_FMT() },

    { { MX_CODEC_VAAPI_420, MX_CODEC_VAAPI_420_10BPP },
                                               FAKE_FMT() },

    { { 0 },                                   FAKE_FMT() }
};

#undef PACKED_FMT
#undef PLANAR_16
#undef PLANAR_8
#undef PLANAR

const MxChromaDescription *mxFourccGetChromaDescription( MxFourcc i_fourcc )
{
    for( unsigned i = 0; p_list_chroma_description[i].p_fourcc[0]; i++ )
    {
        const MxFourcc *p_fourcc = p_list_chroma_description[i].p_fourcc;
        for( unsigned j = 0; j < 4 && p_fourcc[j] != 0; j++ )
        {
            if( p_fourcc[j] == i_fourcc )
                return &p_list_chroma_description[i].description;
        }
    }
    return NULL;
}
