/*****************************************************************************
 * vlc_fourcc.h: Definition of various FOURCC and helpers
 *****************************************************************************
 * Copyright (C) 2009 Laurent Aimar
 * $Id: 486fae1849a9dc9c9ec0d191e33280728987ddb3 $
 *
 * Authors: Laurent Aimar <fenrir _AT_ videolan _DOT_ com>
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

#ifndef MXFOURCC_H
#define MXFOURCC_H 1

typedef uint32_t MxFourcc;
#define MX_CODEC_UNKNOWN         MX_FOURCC('u','n','d','f')

/* Video codec */
#define MX_CODEC_MPGV            MX_FOURCC('m','p','g','v')
#define MX_CODEC_MP4V            MX_FOURCC('m','p','4','v')
#define MX_CODEC_DIV1            MX_FOURCC('D','I','V','1')
#define MX_CODEC_DIV2            MX_FOURCC('D','I','V','2')
#define MX_CODEC_DIV3            MX_FOURCC('D','I','V','3')
#define MX_CODEC_SVQ1            MX_FOURCC('S','V','Q','1')
#define MX_CODEC_SVQ3            MX_FOURCC('S','V','Q','3')
#define MX_CODEC_H264            MX_FOURCC('h','2','6','4')
#define MX_CODEC_H263            MX_FOURCC('h','2','6','3')
#define MX_CODEC_H263I           MX_FOURCC('I','2','6','3')
#define MX_CODEC_H263P           MX_FOURCC('I','L','V','R')
#define MX_CODEC_FLV1            MX_FOURCC('F','L','V','1')
#define MX_CODEC_H261            MX_FOURCC('h','2','6','1')
#define MX_CODEC_MJPG            MX_FOURCC('M','J','P','G')
#define MX_CODEC_MJPGB           MX_FOURCC('m','j','p','b')
#define MX_CODEC_LJPG            MX_FOURCC('L','J','P','G')
#define MX_CODEC_WMV1            MX_FOURCC('W','M','V','1')
#define MX_CODEC_WMV2            MX_FOURCC('W','M','V','2')
#define MX_CODEC_WMV3            MX_FOURCC('W','M','V','3')
#define MX_CODEC_WMVA            MX_FOURCC('W','M','V','A')
#define MX_CODEC_WMVP            MX_FOURCC('W','M','V','P')
#define MX_CODEC_WMVP2           MX_FOURCC('W','V','P','2')
#define MX_CODEC_VC1             MX_FOURCC('V','C','-','1')
#define MX_CODEC_DAALA           MX_FOURCC('d','a','a','l')
#define MX_CODEC_THEORA          MX_FOURCC('t','h','e','o')
#define MX_CODEC_TARKIN          MX_FOURCC('t','a','r','k')
#define MX_CODEC_DIRAC           MX_FOURCC('d','r','a','c')
#define MX_CODEC_OGGSPOTS        MX_FOURCC('S','P','O','T')
#define MX_CODEC_CAVS            MX_FOURCC('C','A','V','S')
#define MX_CODEC_NUV             MX_FOURCC('N','J','P','G')
#define MX_CODEC_RV10            MX_FOURCC('R','V','1','0')
#define MX_CODEC_RV13            MX_FOURCC('R','V','1','3')
#define MX_CODEC_RV20            MX_FOURCC('R','V','2','0')
#define MX_CODEC_RV30            MX_FOURCC('R','V','3','0')
#define MX_CODEC_RV40            MX_FOURCC('R','V','4','0')
#define MX_CODEC_VP3             MX_FOURCC('V','P','3',' ')
#define MX_CODEC_VP5             MX_FOURCC('V','P','5',' ')
#define MX_CODEC_VP6             MX_FOURCC('V','P','6','2')
#define MX_CODEC_VP6F            MX_FOURCC('V','P','6','F')
#define MX_CODEC_VP6A            MX_FOURCC('V','P','6','A')
#define MX_CODEC_MSVIDEO1        MX_FOURCC('M','S','V','C')
#define MX_CODEC_FLIC            MX_FOURCC('F','L','I','C')
#define MX_CODEC_SP5X            MX_FOURCC('S','P','5','X')
#define MX_CODEC_DV              MX_FOURCC('d','v',' ',' ')
#define MX_CODEC_MSRLE           MX_FOURCC('m','r','l','e')
#define MX_CODEC_HUFFYUV         MX_FOURCC('H','F','Y','U')
#define MX_CODEC_FFVHUFF         MX_FOURCC('F','F','V','H')
#define MX_CODEC_ASV1            MX_FOURCC('A','S','V','1')
#define MX_CODEC_ASV2            MX_FOURCC('A','S','V','2')
#define MX_CODEC_FFV1            MX_FOURCC('F','F','V','1')
#define MX_CODEC_VCR1            MX_FOURCC('V','C','R','1')
#define MX_CODEC_CLJR            MX_FOURCC('C','L','J','R')
#define MX_CODEC_RPZA            MX_FOURCC('r','p','z','a')
#define MX_CODEC_SMC             MX_FOURCC('s','m','c',' ')
#define MX_CODEC_CINEPAK         MX_FOURCC('C','V','I','D')
#define MX_CODEC_TSCC            MX_FOURCC('T','S','C','C')
#define MX_CODEC_CSCD            MX_FOURCC('C','S','C','D')
#define MX_CODEC_ZMBV            MX_FOURCC('Z','M','B','V')
#define MX_CODEC_VMNC            MX_FOURCC('V','M','n','c')
#define MX_CODEC_FMVC            MX_FOURCC('F','M','V','C')
#define MX_CODEC_FRAPS           MX_FOURCC('F','P','S','1')
#define MX_CODEC_TRUEMOTION1     MX_FOURCC('D','U','C','K')
#define MX_CODEC_TRUEMOTION2     MX_FOURCC('T','M','2','0')
#define MX_CODEC_QTRLE           MX_FOURCC('r','l','e',' ')
#define MX_CODEC_QDRAW           MX_FOURCC('q','d','r','w')
#define MX_CODEC_QPEG            MX_FOURCC('Q','P','E','G')
#define MX_CODEC_ULTI            MX_FOURCC('U','L','T','I')
#define MX_CODEC_VIXL            MX_FOURCC('V','I','X','L')
#define MX_CODEC_LOCO            MX_FOURCC('L','O','C','O')
#define MX_CODEC_WNV1            MX_FOURCC('W','N','V','1')
#define MX_CODEC_AASC            MX_FOURCC('A','A','S','C')
#define MX_CODEC_INDEO2          MX_FOURCC('I','V','2','0')
#define MX_CODEC_INDEO3          MX_FOURCC('I','V','3','1')
#define MX_CODEC_INDEO4          MX_FOURCC('I','V','4','1')
#define MX_CODEC_INDEO5          MX_FOURCC('I','V','5','0')
#define MX_CODEC_FLASHSV         MX_FOURCC('F','S','V','1')
#define MX_CODEC_KMVC            MX_FOURCC('K','M','V','C')
#define MX_CODEC_SMACKVIDEO      MX_FOURCC('S','M','K','2')
#define MX_CODEC_DNXHD           MX_FOURCC('A','V','d','n')
#define MX_CODEC_8BPS            MX_FOURCC('8','B','P','S')
#define MX_CODEC_MIMIC           MX_FOURCC('M','L','2','O')
#define MX_CODEC_INTERPLAY       MX_FOURCC('i','m','v','e')
#define MX_CODEC_IDCIN           MX_FOURCC('I','D','C','I')
#define MX_CODEC_4XM             MX_FOURCC('4','X','M','V')
#define MX_CODEC_ROQ             MX_FOURCC('R','o','Q','v')
#define MX_CODEC_MDEC            MX_FOURCC('M','D','E','C')
#define MX_CODEC_VMDVIDEO        MX_FOURCC('V','M','D','V')
#define MX_CODEC_CDG             MX_FOURCC('C','D','G',' ')
#define MX_CODEC_FRWU            MX_FOURCC('F','R','W','U')
#define MX_CODEC_AMV             MX_FOURCC('A','M','V',' ')
#define MX_CODEC_VP7             MX_FOURCC('V','P','7','0')
#define MX_CODEC_VP8             MX_FOURCC('V','P','8','0')
#define MX_CODEC_VP9             MX_FOURCC('V','P','9','0')
#define MX_CODEC_VP10            MX_FOURCC('V','P',':','0')
#define MX_CODEC_AV1             MX_FOURCC('a','v','0','1')
#define MX_CODEC_JPEG2000        MX_FOURCC('J','P','2','K')
#define MX_CODEC_LAGARITH        MX_FOURCC('L','A','G','S')
#define MX_CODEC_FLASHSV2        MX_FOURCC('F','S','V','2')
#define MX_CODEC_PRORES          MX_FOURCC('a','p','c','n')
#define MX_CODEC_MXPEG           MX_FOURCC('M','X','P','G')
#define MX_CODEC_CDXL            MX_FOURCC('C','D','X','L')
#define MX_CODEC_BMVVIDEO        MX_FOURCC('B','M','V','V')
#define MX_CODEC_UTVIDEO         MX_FOURCC('U','L','R','A')
#define MX_CODEC_VBLE            MX_FOURCC('V','B','L','E')
#define MX_CODEC_DXTORY          MX_FOURCC('x','t','o','r')
#define MX_CODEC_MSS1            MX_FOURCC('M','S','S','1')
#define MX_CODEC_MSS2            MX_FOURCC('M','S','S','2')
#define MX_CODEC_MSA1            MX_FOURCC('M','S','A','1')
#define MX_CODEC_TSC2            MX_FOURCC('T','S','C','2')
#define MX_CODEC_MTS2            MX_FOURCC('M','T','S','2')
#define MX_CODEC_HEVC            MX_FOURCC('h','e','v','c')
#define MX_CODEC_ICOD            MX_FOURCC('i','c','o','d')
#define MX_CODEC_G2M2            MX_FOURCC('G','2','M','2')
#define MX_CODEC_G2M3            MX_FOURCC('G','2','M','3')
#define MX_CODEC_G2M4            MX_FOURCC('G','2','M','4')
#define MX_CODEC_BINKVIDEO       MX_FOURCC('B','I','K','f')
#define MX_CODEC_BINKAUDIO_DCT   MX_FOURCC('B','A','U','1')
#define MX_CODEC_BINKAUDIO_RDFT  MX_FOURCC('B','A','U','2')
#define MX_CODEC_XAN_WC4         MX_FOURCC('X','x','a','n')
#define MX_CODEC_LCL_MSZH        MX_FOURCC('M','S','Z','H')
#define MX_CODEC_LCL_ZLIB        MX_FOURCC('Z','L','I','B')
#define MX_CODEC_THP             MX_FOURCC('T','H','P','0')
#define MX_CODEC_ESCAPE124       MX_FOURCC('E','1','2','4')
#define MX_CODEC_KGV1            MX_FOURCC('K','G','V','1')
#define MX_CODEC_CLLC            MX_FOURCC('C','L','L','C')
#define MX_CODEC_AURA            MX_FOURCC('A','U','R','A')
#define MX_CODEC_FIC             MX_FOURCC('F','I','C','V')
#define MX_CODEC_TMV             MX_FOURCC('T','M','A','V')
#define MX_CODEC_XAN_WC3         MX_FOURCC('X','A','N','3')
#define MX_CODEC_WS_VQA          MX_FOURCC('W','V','Q','A')
#define MX_CODEC_MMVIDEO         MX_FOURCC('M','M','V','I')
#define MX_CODEC_AVS             MX_FOURCC('A','V','S','V')
#define MX_CODEC_DSICINVIDEO     MX_FOURCC('D','C','I','V')
#define MX_CODEC_TIERTEXSEQVIDEO MX_FOURCC('T','S','E','Q')
#define MX_CODEC_DXA             MX_FOURCC('D','E','X','A')
#define MX_CODEC_C93             MX_FOURCC('I','C','9','3')
#define MX_CODEC_BETHSOFTVID     MX_FOURCC('B','V','I','D')
#define MX_CODEC_VB              MX_FOURCC('V','B','V','1')
#define MX_CODEC_RL2             MX_FOURCC('R','L','V','2')
#define MX_CODEC_BFI             MX_FOURCC('B','F','&','I')
#define MX_CODEC_CMV             MX_FOURCC('E','C','M','V')
#define MX_CODEC_MOTIONPIXELS    MX_FOURCC('M','P','I','X')
#define MX_CODEC_TGV             MX_FOURCC('T','G','V','V')
#define MX_CODEC_TGQ             MX_FOURCC('T','G','Q','V')
#define MX_CODEC_TQI             MX_FOURCC('T','Q','I','V')
#define MX_CODEC_MAD             MX_FOURCC('M','A','D','V')
#define MX_CODEC_ANM             MX_FOURCC('A','N','I','M')
#define MX_CODEC_YOP             MX_FOURCC('Y','O','P','V')
#define MX_CODEC_JV              MX_FOURCC('J','V','0','0')
#define MX_CODEC_DFA             MX_FOURCC('D','F','I','A')
#define MX_CODEC_HNM4_VIDEO      MX_FOURCC('H','N','M','4')
#define MX_CODEC_TDSC            MX_FOURCC('T','D','S','C')
#define MX_CODEC_HQX             MX_FOURCC('C','H','Q','X')
#define MX_CODEC_HQ_HQA          MX_FOURCC('C','U','V','C')
#define MX_CODEC_HAP             MX_FOURCC('H','A','P','1')
#define MX_CODEC_DXV             MX_FOURCC('D','X','D','3')
#define MX_CODEC_CINEFORM        MX_FOURCC('C','F','H','D')
#define MX_CODEC_SPEEDHQ         MX_FOURCC('S','H','Q','2')
#define MX_CODEC_PIXLET          MX_FOURCC('p','x','l','t')

/***********
 * Chromas
 ***********/

/* Planar YUV */

/* Planar YUV 4:1:0 Y:V:U */
#define MX_CODEC_YV9             MX_FOURCC('Y','V','U','9')
/* Planar YUV 4:1:0 Y:U:V */
#define MX_CODEC_I410            MX_FOURCC('I','4','1','0')
/* Planar YUV 4:1:1 Y:U:V */
#define MX_CODEC_I411            MX_FOURCC('I','4','1','1')

/* Planar YUV 4:2:0 Y:V:U */
#define MX_CODEC_YV12            MX_FOURCC('Y','V','1','2')
/* Planar YUV 4:2:0 Y:U:V 8-bit */
#define MX_CODEC_I420            MX_FOURCC('I','4','2','0')
/* Planar YUV 4:2:0 Y:U:V  9-bit stored on 16 bits */
#define MX_CODEC_I420_9L         MX_FOURCC('I','0','9','L')
#define MX_CODEC_I420_9B         MX_FOURCC('I','0','9','B')
/* Planar YUV 4:2:0 Y:U:V 10-bit stored on 16 bits LSB */
#define MX_CODEC_I420_10L        MX_FOURCC('I','0','A','L')
#define MX_CODEC_I420_10B        MX_FOURCC('I','0','A','B')
/* Planar YUV 4:2:0 Y:U:V 12-bit stored on 16 bits */
#define MX_CODEC_I420_12L        MX_FOURCC('I','0','C','L')
#define MX_CODEC_I420_12B        MX_FOURCC('I','0','C','B')

/* Planar YUV 4:2:0 Y:U:V 16-bit stored on 16 bits */
#define MX_CODEC_I420_16L        MX_FOURCC('I','0','F','L')
#define MX_CODEC_I420_16B        MX_FOURCC('I','0','F','B')

/* Planar YUV 4:2:2 Y:U:V 8-bit */
#define MX_CODEC_I422            MX_FOURCC('I','4','2','2')
/* Planar YUV 4:2:2 Y:U:V  9-bit stored on 16 bits */
#define MX_CODEC_I422_9L         MX_FOURCC('I','2','9','L')
#define MX_CODEC_I422_9B         MX_FOURCC('I','2','9','B')
/* Planar YUV 4:2:2 Y:U:V 10-bit stored on 16 bits */
#define MX_CODEC_I422_10L        MX_FOURCC('I','2','A','L')
#define MX_CODEC_I422_10B        MX_FOURCC('I','2','A','B')
/* Planar YUV 4:2:2 Y:U:V 12-bit stored on 16 bits */
#define MX_CODEC_I422_12L        MX_FOURCC('I','2','C','L')
#define MX_CODEC_I422_12B        MX_FOURCC('I','2','C','B')

/* Planar YUV 4:4:0 Y:U:V */
#define MX_CODEC_I440            MX_FOURCC('I','4','4','0')
/* Planar YUV 4:4:4 Y:U:V 8-bit */
#define MX_CODEC_I444            MX_FOURCC('I','4','4','4')
/* Planar YUV 4:4:4 Y:U:V  9-bit stored on 16 bits */
#define MX_CODEC_I444_9L         MX_FOURCC('I','4','9','L')
#define MX_CODEC_I444_9B         MX_FOURCC('I','4','9','B')
/* Planar YUV 4:4:4 Y:U:V 10-bit stored on 16 bits */
#define MX_CODEC_I444_10L        MX_FOURCC('I','4','A','L')
#define MX_CODEC_I444_10B        MX_FOURCC('I','4','A','B')
/* Planar YUV 4:4:4 Y:U:V 12-bit stored on 16 bits */
#define MX_CODEC_I444_12L        MX_FOURCC('I','4','C','L')
#define MX_CODEC_I444_12B        MX_FOURCC('I','4','C','B')
/* Planar YUV 4:4:4 Y:U:V 16-bit */
#define MX_CODEC_I444_16L        MX_FOURCC('I','4','F','L')
#define MX_CODEC_I444_16B        MX_FOURCC('I','4','F','B')

/* Planar YUV 4:2:0 Y:U:V full scale */
#define MX_CODEC_J420            MX_FOURCC('J','4','2','0')
/* Planar YUV 4:2:2 Y:U:V full scale */
#define MX_CODEC_J422            MX_FOURCC('J','4','2','2')
/* Planar YUV 4:4:0 Y:U:V full scale */
#define MX_CODEC_J440            MX_FOURCC('J','4','4','0')
/* Planar YUV 4:4:4 Y:U:V full scale */
#define MX_CODEC_J444            MX_FOURCC('J','4','4','4')
/* Palettized YUV with palette element Y:U:V:A */
#define MX_CODEC_YUVP            MX_FOURCC('Y','U','V','P')

/* Planar YUV 4:4:4 Y:U:V:A */
#define MX_CODEC_YUVA            MX_FOURCC('Y','U','V','A')
/* Planar YUV 4:2:2 Y:U:V:A */
#define MX_CODEC_YUV422A         MX_FOURCC('I','4','2','A')
/* Planar YUV 4:2:0 Y:U:V:A */
#define MX_CODEC_YUV420A         MX_FOURCC('I','4','0','A')

/* Planar Y:U:V:A 4:4:4 10bits */
#define MX_CODEC_YUVA_444_10L    MX_FOURCC('Y','A','0','L')
#define MX_CODEC_YUVA_444_10B    MX_FOURCC('Y','A','0','B')

/* Semi-planar Y/UV */

/* 2 planes Y/UV 4:2:0 */
#define MX_CODEC_NV12            MX_FOURCC('N','V','1','2')
/* 2 planes Y/VU 4:2:0 */
#define MX_CODEC_NV21            MX_FOURCC('N','V','2','1')
/* 2 planes Y/UV 4:2:2 */
#define MX_CODEC_NV16            MX_FOURCC('N','V','1','6')
/* 2 planes Y/VU 4:2:2 */
#define MX_CODEC_NV61            MX_FOURCC('N','V','6','1')
/* 2 planes Y/UV 4:4:4 */
#define MX_CODEC_NV24            MX_FOURCC('N','V','2','4')
/* 2 planes Y/VU 4:4:4 */
#define MX_CODEC_NV42            MX_FOURCC('N','V','4','2')
/* 2 planes Y/UV 4:2:0 10-bit */
#define MX_CODEC_P010            MX_FOURCC('P','0','1','0')

/* Packed YUV */

/* Packed YUV 4:2:2, U:Y:V:Y */
#define MX_CODEC_UYVY            MX_FOURCC('U','Y','V','Y')
/* Packed YUV 4:2:2, V:Y:U:Y */
#define MX_CODEC_VYUY            MX_FOURCC('V','Y','U','Y')
/* Packed YUV 4:2:2, Y:U:Y:V */
#define MX_CODEC_YUYV            MX_FOURCC('Y','U','Y','2')
/* Packed YUV 4:2:2, Y:V:Y:U */
#define MX_CODEC_YVYU            MX_FOURCC('Y','V','Y','U')
/* Packed YUV 2:1:1, Y:U:Y:V */
#define MX_CODEC_Y211            MX_FOURCC('Y','2','1','1')
/* Packed YUV 4:2:2, U:Y:V:Y, reverted */
#define MX_CODEC_CYUV            MX_FOURCC('c','y','u','v')
/* 10-bit 4:2:2 Component YCbCr */
#define MX_CODEC_V210            MX_FOURCC('v','2','1','0')
/* I420 packed for RTP (RFC 4175) */
#define MX_CODEC_R420            MX_FOURCC('r','4','2','0')


/* RGB */

/* Palettized RGB with palette element R:G:B */
#define MX_CODEC_RGBP            MX_FOURCC('R','G','B','P')
/* 8 bits RGB */
#define MX_CODEC_RGB8            MX_FOURCC('R','G','B','8')
/* 12 bits RGB padded to 16 bits */
#define MX_CODEC_RGB12           MX_FOURCC('R','V','1','2')
/* 15 bits RGB padded to 16 bits */
#define MX_CODEC_RGB15           MX_FOURCC('R','V','1','5')
/* 16 bits RGB */
#define MX_CODEC_RGB16           MX_FOURCC('R','V','1','6')
/* 24 bits RGB */
#define MX_CODEC_RGB24           MX_FOURCC('R','V','2','4')
/* 24 bits RGB padded to 32 bits */
#define MX_CODEC_RGB32           MX_FOURCC('R','V','3','2')
/* 32 bits RGBA */
#define MX_CODEC_RGBA            MX_FOURCC('R','G','B','A')
/* 32 bits ARGB */
#define MX_CODEC_ARGB            MX_FOURCC('A','R','G','B')
/* 32 bits BGRA */
#define MX_CODEC_BGRA            MX_FOURCC('B','G','R','A')

/* Planar GBR 4:4:4 8 bits */
#define MX_CODEC_GBR_PLANAR      MX_FOURCC('G','B','R','8')
#define MX_CODEC_GBR_PLANAR_9B   MX_FOURCC('G','B','9','B')
#define MX_CODEC_GBR_PLANAR_9L   MX_FOURCC('G','B','9','L')
#define MX_CODEC_GBR_PLANAR_10B  MX_FOURCC('G','B','A','B')
#define MX_CODEC_GBR_PLANAR_10L  MX_FOURCC('G','B','A','L')
#define MX_CODEC_GBR_PLANAR_16L  MX_FOURCC('G','B','F','L')
#define MX_CODEC_GBR_PLANAR_16B  MX_FOURCC('G','B','F','B')

/* 8 bits grey */
#define MX_CODEC_GREY            MX_FOURCC('G','R','E','Y')

/* VDPAU video surface YCbCr 4:2:0 */
#define MX_CODEC_VDPAU_VIDEO_420 MX_FOURCC('V','D','V','0')
/* VDPAU video surface YCbCr 4:2:2 */
#define MX_CODEC_VDPAU_VIDEO_422 MX_FOURCC('V','D','V','2')
/* VDPAU video surface YCbCr 4:4:4 */
#define MX_CODEC_VDPAU_VIDEO_444 MX_FOURCC('V','D','V','4')
/* VDPAU output surface RGBA */
#define MX_CODEC_VDPAU_OUTPUT    MX_FOURCC('V','D','O','R')

/* VAAPI opaque surface */
#define MX_CODEC_VAAPI_420 MX_FOURCC('V','A','O','P') /* 4:2:0  8 bpc */
#define MX_CODEC_VAAPI_420_10BPP MX_FOURCC('V','A','O','0') /* 4:2:0 10 bpc */

/* MediaCodec/IOMX opaque buffer type */
#define MX_CODEC_ANDROID_OPAQUE  MX_FOURCC('A','N','O','P')

/* Broadcom MMAL opaque buffer type */
#define MX_CODEC_MMAL_OPAQUE     MX_FOURCC('M','M','A','L')

/* DXVA2 opaque video surface for use with D3D9 */
#define MX_CODEC_D3D9_OPAQUE     MX_FOURCC('D','X','A','9') /* 4:2:0  8 bpc */
#define MX_CODEC_D3D9_OPAQUE_10B MX_FOURCC('D','X','A','0') /* 4:2:0 10 bpc */

/* D3D11VA opaque video surface for use with D3D11 */
#define MX_CODEC_D3D11_OPAQUE          MX_FOURCC('D','X','1','1') /* 4:2:0  8 bpc */
#define MX_CODEC_D3D11_OPAQUE_10B      MX_FOURCC('D','X','1','0') /* 4:2:0 10 bpc */

/* CVPixelBuffer opaque buffer type */
#define MX_CODEC_CVPX_NV12       MX_FOURCC('C','V','P','N')
#define MX_CODEC_CVPX_UYVY       MX_FOURCC('C','V','P','Y')
#define MX_CODEC_CVPX_I420       MX_FOURCC('C','V','P','I')
#define MX_CODEC_CVPX_BGRA       MX_FOURCC('C','V','P','B')
#define MX_CODEC_CVPX_P010       MX_FOURCC('C','V','P','P')

/* Image codec (video) */
#define MX_CODEC_PNG             MX_FOURCC('p','n','g',' ')
#define MX_CODEC_PPM             MX_FOURCC('p','p','m',' ')
#define MX_CODEC_PGM             MX_FOURCC('p','g','m',' ')
#define MX_CODEC_PGMYUV          MX_FOURCC('p','g','m','y')
#define MX_CODEC_PAM             MX_FOURCC('p','a','m',' ')
#define MX_CODEC_JPEG            MX_FOURCC('j','p','e','g')
#define MX_CODEC_BPG             MX_FOURCC('B','P','G',0xFB)
#define MX_CODEC_JPEGLS          MX_FOURCC('M','J','L','S')
#define MX_CODEC_BMP             MX_FOURCC('b','m','p',' ')
#define MX_CODEC_TIFF            MX_FOURCC('t','i','f','f')
#define MX_CODEC_GIF             MX_FOURCC('g','i','f',' ')
#define MX_CODEC_TARGA           MX_FOURCC('t','g','a',' ')
#define MX_CODEC_SVG             MX_FOURCC('s','v','g',' ')
#define MX_CODEC_SGI             MX_FOURCC('s','g','i',' ')
#define MX_CODEC_PNM             MX_FOURCC('p','n','m',' ')
#define MX_CODEC_PCX             MX_FOURCC('p','c','x',' ')
#define MX_CODEC_XWD             MX_FOURCC('X','W','D',' ')
#define MX_CODEC_TXD             MX_FOURCC('T','X','D',' ')


/* Audio codec */
#define MX_CODEC_MPGA                       MX_FOURCC('m','p','g','a')
#define MX_CODEC_MP4A                       MX_FOURCC('m','p','4','a')
#define MX_CODEC_ALS                        MX_FOURCC('a','l','s',' ')
#define MX_CODEC_A52                        MX_FOURCC('a','5','2',' ')
#define MX_CODEC_EAC3                       MX_FOURCC('e','a','c','3')
#define MX_CODEC_DTS                        MX_FOURCC('d','t','s',' ')
#define MX_CODEC_WMA1                       MX_FOURCC('W','M','A','1')
#define MX_CODEC_WMA2                       MX_FOURCC('W','M','A','2')
#define MX_CODEC_WMAP                       MX_FOURCC('W','M','A','P')
#define MX_CODEC_WMAL                       MX_FOURCC('W','M','A','L')
#define MX_CODEC_WMAS                       MX_FOURCC('W','M','A','S')
#define MX_CODEC_FLAC                       MX_FOURCC('f','l','a','c')
#define MX_CODEC_MLP                        MX_FOURCC('m','l','p',' ')
#define MX_CODEC_TRUEHD                     MX_FOURCC('t','r','h','d')
#define MX_CODEC_DVAUDIO                    MX_FOURCC('d','v','a','u')
#define MX_CODEC_SPEEX                      MX_FOURCC('s','p','x',' ')
#define MX_CODEC_OPUS                       MX_FOURCC('O','p','u','s')
#define MX_CODEC_VORBIS                     MX_FOURCC('v','o','r','b')
#define MX_CODEC_MACE3                      MX_FOURCC('M','A','C','3')
#define MX_CODEC_MACE6                      MX_FOURCC('M','A','C','6')
#define MX_CODEC_MUSEPACK7                  MX_FOURCC('M','P','C',' ')
#define MX_CODEC_MUSEPACK8                  MX_FOURCC('M','P','C','K')
#define MX_CODEC_RA_144                     MX_FOURCC('1','4','_','4')
#define MX_CODEC_RA_288                     MX_FOURCC('2','8','_','8')
#define MX_CODEC_INTERPLAY_DPCM             MX_FOURCC('i','d','p','c')
#define MX_CODEC_ROQ_DPCM                   MX_FOURCC('R','o','Q','a')
#define MX_CODEC_DSICINAUDIO                MX_FOURCC('D','C','I','A')
#define MX_CODEC_ADPCM_4XM                  MX_FOURCC('4','x','m','a')
#define MX_CODEC_ADPCM_EA                   MX_FOURCC('A','D','E','A')
#define MX_CODEC_ADPCM_XA                   MX_FOURCC('x','a',' ',' ')
#define MX_CODEC_ADPCM_ADX                  MX_FOURCC('a','d','x',' ')
#define MX_CODEC_ADPCM_IMA_WS               MX_FOURCC('A','I','W','S')
#define MX_CODEC_ADPCM_G722                 MX_FOURCC('g','7','2','2')
#define MX_CODEC_ADPCM_G726                 MX_FOURCC('g','7','2','6')
#define MX_CODEC_ADPCM_SWF                  MX_FOURCC('S','W','F','a')
#define MX_CODEC_ADPCM_MS                   MX_FOURCC('m','s',0x00,0x02)
#define MX_CODEC_ADPCM_IMA_WAV              MX_FOURCC('m','s',0x00,0x11)
#define MX_CODEC_ADPCM_IMA_AMV              MX_FOURCC('i','m','a','v')
#define MX_CODEC_ADPCM_IMA_QT               MX_FOURCC('i','m','a','4')
#define MX_CODEC_ADPCM_YAMAHA               MX_FOURCC('m','s',0x00,0x20)
#define MX_CODEC_ADPCM_DK3                  MX_FOURCC('m','s',0x00,0x62)
#define MX_CODEC_ADPCM_DK4                  MX_FOURCC('m','s',0x00,0x61)
#define MX_CODEC_ADPCM_CREATIVE             MX_FOURCC('m','s',0x00,0xC0)
#define MX_CODEC_ADPCM_SBPRO_2              MX_FOURCC('m','s',0x00,0xC2)
#define MX_CODEC_ADPCM_SBPRO_3              MX_FOURCC('m','s',0x00,0xC3)
#define MX_CODEC_ADPCM_SBPRO_4              MX_FOURCC('m','s',0x00,0xC4)
#define MX_CODEC_ADPCM_THP                  MX_FOURCC('T','H','P','A')
#define MX_CODEC_ADPCM_XA_EA                MX_FOURCC('X','A','J', 0)
#define MX_CODEC_G723_1                     MX_FOURCC('g','7','2', 0x31)
#define MX_CODEC_G729                       MX_FOURCC('g','7','2','9')
#define MX_CODEC_VMDAUDIO                   MX_FOURCC('v','m','d','a')
#define MX_CODEC_AMR_NB                     MX_FOURCC('s','a','m','r')
#define MX_CODEC_AMR_WB                     MX_FOURCC('s','a','w','b')
#define MX_CODEC_ALAC                       MX_FOURCC('a','l','a','c')
#define MX_CODEC_QDM2                       MX_FOURCC('Q','D','M','2')
#define MX_CODEC_QDMC                       MX_FOURCC('Q','D','M','C')
#define MX_CODEC_COOK                       MX_FOURCC('c','o','o','k')
#define MX_CODEC_SIPR                       MX_FOURCC('s','i','p','r')
#define MX_CODEC_TTA                        MX_FOURCC('T','T','A','1')
#define MX_CODEC_SHORTEN                    MX_FOURCC('s','h','n',' ')
#define MX_CODEC_WAVPACK                    MX_FOURCC('W','V','P','K')
#define MX_CODEC_GSM                        MX_FOURCC('g','s','m',' ')
#define MX_CODEC_GSM_MS                     MX_FOURCC('a','g','s','m')
#define MX_CODEC_ATRAC1                     MX_FOURCC('a','t','r','1')
#define MX_CODEC_ATRAC3                     MX_FOURCC('a','t','r','c')
#define MX_CODEC_ATRAC3P                    MX_FOURCC('a','t','r','p')
#define MX_CODEC_IMC                        MX_FOURCC(0x1,0x4,0x0,0x0)
#define MX_CODEC_TRUESPEECH                 MX_FOURCC(0x22,0x0,0x0,0x0)
#define MX_CODEC_NELLYMOSER                 MX_FOURCC('N','E','L','L')
#define MX_CODEC_APE                        MX_FOURCC('A','P','E',' ')
#define MX_CODEC_QCELP                      MX_FOURCC('Q','c','l','p')
#define MX_CODEC_302M                       MX_FOURCC('3','0','2','m')
#define MX_CODEC_DVD_LPCM                   MX_FOURCC('l','p','c','m')
#define MX_CODEC_DVDA_LPCM                  MX_FOURCC('a','p','c','m')
#define MX_CODEC_BD_LPCM                    MX_FOURCC('b','p','c','m')
#define MX_CODEC_WIDI_LPCM                  MX_FOURCC('w','p','c','m')
#define MX_CODEC_SDDS                       MX_FOURCC('s','d','d','s')
#define MX_CODEC_MIDI                       MX_FOURCC('M','I','D','I')
#define MX_CODEC_RALF                       MX_FOURCC('R','A','L','F')

#define MX_CODEC_S8                         MX_FOURCC('s','8',' ',' ')
#define MX_CODEC_U8                         MX_FOURCC('u','8',' ',' ')
#define MX_CODEC_S16L                       MX_FOURCC('s','1','6','l')
#define MX_CODEC_S16L_PLANAR                MX_FOURCC('s','1','l','p')
#define MX_CODEC_S16B                       MX_FOURCC('s','1','6','b')
#define MX_CODEC_U16L                       MX_FOURCC('u','1','6','l')
#define MX_CODEC_U16B                       MX_FOURCC('u','1','6','b')
#define MX_CODEC_S20B                       MX_FOURCC('s','2','0','b')
#define MX_CODEC_S24L                       MX_FOURCC('s','2','4','l')
#define MX_CODEC_S24B                       MX_FOURCC('s','2','4','b')
#define MX_CODEC_U24L                       MX_FOURCC('u','2','4','l')
#define MX_CODEC_U24B                       MX_FOURCC('u','2','4','b')
#define MX_CODEC_S24L32                     MX_FOURCC('s','2','4','4')
#define MX_CODEC_S24B32                     MX_FOURCC('S','2','4','4')
#define MX_CODEC_S32L                       MX_FOURCC('s','3','2','l')
#define MX_CODEC_S32B                       MX_FOURCC('s','3','2','b')
#define MX_CODEC_U32L                       MX_FOURCC('u','3','2','l')
#define MX_CODEC_U32B                       MX_FOURCC('u','3','2','b')
#define MX_CODEC_F32L                       MX_FOURCC('f','3','2','l')
#define MX_CODEC_F32B                       MX_FOURCC('f','3','2','b')
#define MX_CODEC_F64L                       MX_FOURCC('f','6','4','l')
#define MX_CODEC_F64B                       MX_FOURCC('f','6','4','b')

#define MX_CODEC_ALAW                       MX_FOURCC('a','l','a','w')
#define MX_CODEC_MULAW                      MX_FOURCC('m','l','a','w')
#define MX_CODEC_DAT12                      MX_FOURCC('L','P','1','2')
#define MX_CODEC_S24DAUD                    MX_FOURCC('d','a','u','d')
#define MX_CODEC_TWINVQ                     MX_FOURCC('T','W','I','N')
#define MX_CODEC_BMVAUDIO                   MX_FOURCC('B','M','V','A')
#define MX_CODEC_ULEAD_DV_AUDIO_NTSC        MX_FOURCC('m','s',0x02,0x15)
#define MX_CODEC_ULEAD_DV_AUDIO_PAL         MX_FOURCC('m','s',0x02,0x16)
#define MX_CODEC_INDEO_AUDIO                MX_FOURCC('m','s',0x04,0x02)
#define MX_CODEC_METASOUND                  MX_FOURCC('m','s',0x00,0x75)
#define MX_CODEC_ON2AVC                     MX_FOURCC('m','s',0x05,0x00)
#define MX_CODEC_TAK                        MX_FOURCC('t','a','k',' ')
#define MX_CODEC_SMACKAUDIO                 MX_FOURCC('S','M','K','A')
#define MX_CODEC_ADPCM_IMA_EA_SEAD          MX_FOURCC('S','E','A','D')
#define MX_CODEC_ADPCM_EA_R1                MX_FOURCC('E','A','R','1')
#define MX_CODEC_ADPCM_IMA_APC              MX_FOURCC('A','I','P','C')

/* Subtitle */
#define MX_CODEC_SPU       MX_FOURCC('s','p','u',' ')
#define MX_CODEC_DVBS      MX_FOURCC('d','v','b','s')
#define MX_CODEC_SUBT      MX_FOURCC('s','u','b','t')
#define MX_CODEC_XSUB      MX_FOURCC('X','S','U','B')
#define MX_CODEC_SSA       MX_FOURCC('s','s','a',' ')
#define MX_CODEC_TEXT      MX_FOURCC('T','E','X','T')
#define MX_CODEC_TELETEXT  MX_FOURCC('t','e','l','x')
#define MX_CODEC_KATE      MX_FOURCC('k','a','t','e')
#define MX_CODEC_CMML      MX_FOURCC('c','m','m','l')
#define MX_CODEC_ITU_T140  MX_FOURCC('t','1','4','0')
#define MX_CODEC_USF       MX_FOURCC('u','s','f',' ')
#define MX_CODEC_OGT       MX_FOURCC('o','g','t',' ')
#define MX_CODEC_CVD       MX_FOURCC('c','v','d',' ')
#define MX_CODEC_TX3G      MX_FOURCC('t','x','3','g')
#define MX_CODEC_ARIB_A    MX_FOURCC('a','r','b','a')
#define MX_CODEC_ARIB_C    MX_FOURCC('a','r','b','c')
/* Blu-ray Presentation Graphics */
#define MX_CODEC_BD_PG     MX_FOURCC('b','d','p','g')
#define MX_CODEC_BD_TEXT   MX_FOURCC('b','d','t','x')
/* EBU STL (TECH. 3264-E) */
#define MX_CODEC_EBU_STL   MX_FOURCC('S','T','L',' ')
#define MX_CODEC_SCTE_18   MX_FOURCC('S','C','1','8')
#define MX_CODEC_SCTE_27   MX_FOURCC('S','C','2','7')
/* EIA/CEA-608/708 */
#define MX_CODEC_CEA608    MX_FOURCC('c','6','0','8')
#define MX_CODEC_CEA708    MX_FOURCC('c','7','0','8')
#define MX_CODEC_TTML      MX_FOURCC('T','T','M','L')
#define MX_CODEC_WEBVTT    MX_FOURCC('w','v','t','t')

/* XYZ colorspace 12 bits packed in 16 bits, organisation |XXX0|YYY0|ZZZ0| */
#define MX_CODEC_XYZ12     MX_FOURCC('X','Y','1','2')


/* Special endian dependent values
 * The suffic N means Native
 * The suffix I means Inverted (ie non native) */
#ifdef WORDS_BIGENDIAN
#   define MX_CODEC_S16N MX_CODEC_S16B
#   define MX_CODEC_U16N MX_CODEC_U16B
#   define MX_CODEC_S24N MX_CODEC_S24B
#   define MX_CODEC_U24N MX_CODEC_U24B
#   define MX_CODEC_S32N MX_CODEC_S32B
#   define MX_CODEC_U32N MX_CODEC_U32B
#   define MX_CODEC_FL32 MX_CODEC_F32B
#   define MX_CODEC_FL64 MX_CODEC_F64B

#   define MX_CODEC_S16I MX_CODEC_S16L
#   define MX_CODEC_U16I MX_CODEC_U16L
#   define MX_CODEC_S24I MX_CODEC_S24L
#   define MX_CODEC_U24I MX_CODEC_U24L
#   define MX_CODEC_S32I MX_CODEC_S32L
#   define MX_CODEC_U32I MX_CODEC_U32L

#else
#   define MX_CODEC_S16N MX_CODEC_S16L
#   define MX_CODEC_U16N MX_CODEC_U16L
#   define MX_CODEC_S24N MX_CODEC_S24L
#   define MX_CODEC_U24N MX_CODEC_U24L
#   define MX_CODEC_S32N MX_CODEC_S32L
#   define MX_CODEC_U32N MX_CODEC_U32L
#   define MX_CODEC_FL32 MX_CODEC_F32L
#   define MX_CODEC_FL64 MX_CODEC_F64L

#   define MX_CODEC_S16I MX_CODEC_S16B
#   define MX_CODEC_U16I MX_CODEC_U16B
#   define MX_CODEC_S24I MX_CODEC_S24B
#   define MX_CODEC_U24I MX_CODEC_U24B
#   define MX_CODEC_S32I MX_CODEC_S32B
#   define MX_CODEC_U32I MX_CODEC_U32B
#endif

/* Non official codecs, used to force a profile in an encoder */
/* MPEG-1 video */
#define MX_CODEC_MP1V      MX_FOURCC('m','p','1','v')
/* MPEG-2 video */
#define MX_CODEC_MP2V      MX_FOURCC('m','p','2','v')
/* MPEG-I/II layer 2 audio */
#define MX_CODEC_MP2       MX_FOURCC('m','p','2',' ')
/* MPEG-I/II layer 3 audio */
#define MX_CODEC_MP3       MX_FOURCC('m','p','3',' ')

/**
 * It returns the codec associated to a fourcc within an ES category.
 *
 * If not found, it will return the given fourcc.
 * If found, it will always be one of the MX_CODEC_ defined above.
 *
 * You may use UNKNOWN_ES for the ES category if you don't have the information.
 */
MXCODEC_API MxFourcc mxFourccGetCodec( int i_cat, MxFourcc i_fourcc );

/**
 * It returns the codec associated to a fourcc stored in a zero terminated
 * string.
 *
 * If the string is NULL or does not have exactly 4 characters, it will
 * return 0, otherwise it behaves like mxFourccGetCodec.
 *
 * Provided for convenience.
 */
MXCODEC_API MxFourcc mxFourccGetCodecFromString( int i_cat, const char * );

/**
 * It converts the given fourcc to an audio codec when possible.
 *
 * The fourccs converted are aflt, araw/pcm , twos, sowt. When an incompatible i_bits
 * is detected, 0 is returned.
 * The other fourccs go through mxFourccGetCodec and i_bits is not checked.
 */
MXCODEC_API MxFourcc mxFourccGetCodecAudio( MxFourcc i_fourcc, int i_bits );

/**
 * It returns the description of the given fourcc or NULL if not found.
 *
 * You may use UNKNOWN_ES for the ES category if you don't have the information.
 */
MXCODEC_API const char * mxFourccGetDescription( int i_cat, MxFourcc i_fourcc );

/**
 * It returns a list (terminated with the value 0) of YUV fourccs in
 * decreasing priority order for the given chroma.
 *
 * It will always return a non NULL pointer that must not be freed.
 */
MXCODEC_API const MxFourcc * mxFourccGetYUVFallback( MxFourcc );

/**
 * It returns a list (terminated with the value 0) of RGB fourccs in
 * decreasing priority order for the given chroma.
 *
 * It will always return a non NULL pointer that must not be freed.
 */
MXCODEC_API const MxFourcc * mxFourccGetRGBFallback( MxFourcc );

/**
 * It returns true if the given fourcc is YUV and false otherwise.
 */
MXCODEC_API bool mxFourccIsYUV( MxFourcc );

/**
 * It returns true if the two fourccs are equivalent if their U&V planes are
 * swapped.
 */
MXCODEC_API bool mxFourccAreUVPlanesSwapped(MxFourcc , MxFourcc );

/**
 * Chroma related information.
 */
typedef struct {
    unsigned num, den;
} MxRational;

typedef struct {
    unsigned plane_count;
    struct {
        MxRational w;
        MxRational h;
    } p[4];
    unsigned pixel_size;        /* Number of bytes per pixel for a plane */
    unsigned pixel_bits;        /* Number of bits actually used bits per pixel for a plane */
} MxChromaDescription;

/**
 * It returns a MxChroma describing the requested fourcc or NULL
 * if not found.
 */
MXCODEC_API const MxChromaDescription * mxFourccGetChromaDescription( MxFourcc fourcc );

#endif /* MXFOURCC_H */

