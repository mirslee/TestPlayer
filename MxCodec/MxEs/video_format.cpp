#include "stdafx.h"
#include "MxConfig.h"
#include <assert.h>
#include "MxCommon.h"
#include "MxEs.h"
#include "MxFourcc.h"


/*****************************************************************************
 * BinaryLog: computes the base 2 log of a binary value
 *****************************************************************************
 * This functions is used by MaskToShift, to get a bit index from a binary
 * value.
 *****************************************************************************/
static int BinaryLog( uint32_t i )
{
    int i_log = 0;

    if( i == 0 ) return -31337;

    if( i & 0xffff0000 ) i_log += 16;
    if( i & 0xff00ff00 ) i_log += 8;
    if( i & 0xf0f0f0f0 ) i_log += 4;
    if( i & 0xcccccccc ) i_log += 2;
    if( i & 0xaaaaaaaa ) i_log += 1;

    return i_log;
}

/**
 * It transforms a color mask into right and left shifts
 * FIXME copied from video_output.c
 */
static void MaskToShift( int *pi_left, int *pi_right, uint32_t i_mask )
{
    uint32_t i_low, i_high;            /* lower and higher bits of the mask */

    if( !i_mask )
    {
        *pi_left = *pi_right = 0;
        return;
    }

    /* Get bits */
    i_low = i_high = i_mask;

    i_low &= - (int32_t)i_low;          /* lower bit of the mask */
    i_high += i_low;                    /* higher bit of the mask */

    /* Transform bits into an index. Also deal with i_high overflow, which
     * is faster than changing the BinaryLog code to handle 64 bit integers. */
    i_low =  BinaryLog (i_low);
    i_high = i_high ? BinaryLog (i_high) : 32;

    /* Update pointers and return */
    *pi_left =   i_low;
    *pi_right = (8 - i_high + i_low);
}


//Simplify transforms to have something more manageable. Order: angle, hflip.
static void transform_GetBasicOps( video_transform_t transform,
                                  unsigned *restrict angle,
                                  bool *restrict hflip )
{
    *hflip = ORIENT_IS_MIRROR(transform);
    
    switch ( transform )
    {
        case TRANSFORM_R90:
        case TRANSFORM_TRANSPOSE:
            *angle = 90;
            break;
        case TRANSFORM_R180:
        case TRANSFORM_VFLIP:
            *angle = 180;
            break;
        case TRANSFORM_R270:
        case TRANSFORM_ANTI_TRANSPOSE:
            *angle = 270;
            break;
        case TRANSFORM_HFLIP:
        case TRANSFORM_IDENTITY:
            *angle = 0;
            break;
        default:
            vlc_assert_unreachable ();
    }
}

static video_transform_t transform_FromBasicOps( unsigned angle, bool hflip )
{
    switch ( angle )
    {
        case 90:
            return hflip ? TRANSFORM_TRANSPOSE : TRANSFORM_R90;
        case 180:
            return hflip ? TRANSFORM_VFLIP : TRANSFORM_R180;
        case 270:
            return hflip ? TRANSFORM_ANTI_TRANSPOSE : TRANSFORM_R270;
        default:
            return hflip ? TRANSFORM_HFLIP : TRANSFORM_IDENTITY;
    }
}

/* */
void MxVideoFormat::fixRgb()
{
    /* FIXME find right default mask */
    if( !this->data.i_rmask || !this->data.i_gmask || !this->data.i_bmask )
    {
        switch( this->data.i_chroma )
        {
        case MX_CODEC_RGB15:
            this->data.i_rmask = 0x7c00;
            this->data.i_gmask = 0x03e0;
            this->data.i_bmask = 0x001f;
            break;

        case MX_CODEC_RGB16:
            this->data.i_rmask = 0xf800;
            this->data.i_gmask = 0x07e0;
            this->data.i_bmask = 0x001f;
            break;

        case MX_CODEC_RGB24:
            this->data.i_rmask = 0xff0000;
            this->data.i_gmask = 0x00ff00;
            this->data.i_bmask = 0x0000ff;
            break;
        case MX_CODEC_RGB32:
            this->data.i_rmask = 0x00ff0000;
            this->data.i_gmask = 0x0000ff00;
            this->data.i_bmask = 0x000000ff;
            break;

        default:
            return;
        }
    }

    MaskToShift( &this->data.i_lrshift, &this->data.i_rrshift,
                 this->data.i_rmask );
    MaskToShift( &this->data.i_lgshift, &this->data.i_rgshift,
                 this->data.i_gmask );
    MaskToShift( &this->data.i_lbshift, &this->data.i_rbshift,
                 this->data.i_bmask );
}

void MxVideoFormat::setup(MxFourcc i_chroma,
                         int i_width, int i_height,
                         int i_visible_width, int i_visible_height,
                         int i_sar_num, int i_sar_den )
{
    this->data.i_chroma         = mxFourccGetCodec( VIDEO_ES, i_chroma );
    this->data.i_width          = i_width;
    this->data.i_visible_width  = i_visible_width;
    this->data.i_height         = i_height;
    this->data.i_visible_height = i_visible_height;
    this->data.i_x_offset       =
    this->data.i_y_offset       = 0;
    mxUreduce( &this->data.i_sar_num, &this->data.i_sar_den, i_sar_num, i_sar_den, 0 );

    switch( this->data.i_chroma )
    {
    case MX_CODEC_YUVA:
        this->data.i_bits_per_pixel = 32;
        break;
    case MX_CODEC_YUV420A:
        this->data.i_bits_per_pixel = 20;
        break;
    case MX_CODEC_YUV422A:
        this->data.i_bits_per_pixel = 24;
        break;
    case MX_CODEC_I444:
    case MX_CODEC_J444:
        this->data.i_bits_per_pixel = 24;
        break;
    case MX_CODEC_I422:
    case MX_CODEC_YUYV:
    case MX_CODEC_YVYU:
    case MX_CODEC_UYVY:
    case MX_CODEC_VYUY:
    case MX_CODEC_J422:
        this->data.i_bits_per_pixel = 16;
        break;
    case MX_CODEC_I440:
    case MX_CODEC_J440:
        this->data.i_bits_per_pixel = 16;
        break;
    case MX_CODEC_P010:
        this->data.i_bits_per_pixel = 15;
        break;
    case MX_CODEC_I411:
    case MX_CODEC_YV12:
    case MX_CODEC_I420:
    case MX_CODEC_J420:
    case MX_CODEC_NV12:
        this->data.i_bits_per_pixel = 12;
        break;
    case MX_CODEC_YV9:
    case MX_CODEC_I410:
        this->data.i_bits_per_pixel = 9;
        break;
    case MX_CODEC_Y211:
        this->data.i_bits_per_pixel = 8;
        break;
    case MX_CODEC_YUVP:
        this->data.i_bits_per_pixel = 8;
        break;

    case MX_CODEC_RGB32:
    case MX_CODEC_RGBA:
    case MX_CODEC_ARGB:
    case MX_CODEC_BGRA:
        this->data.i_bits_per_pixel = 32;
        break;
    case MX_CODEC_RGB24:
        this->data.i_bits_per_pixel = 24;
        break;
    case MX_CODEC_RGB15:
    case MX_CODEC_RGB16:
        this->data.i_bits_per_pixel = 16;
        break;
    case MX_CODEC_RGB8:
        this->data.i_bits_per_pixel = 8;
        break;

    case MX_CODEC_GREY:
    case MX_CODEC_RGBP:
        this->data.i_bits_per_pixel = 8;
        break;

    case MX_CODEC_XYZ12:
        this->data.i_bits_per_pixel = 48;
        break;

    default:
        this->data.i_bits_per_pixel = 0;
        break;
    }
}

void MxVideoFormat::copyCrop(const MxVideoFormat *p_src )
{
    this->data.i_x_offset       = p_src->data.i_x_offset;
    this->data.i_y_offset       = p_src->data.i_y_offset;
    this->data.i_visible_width  = p_src->data.i_visible_width;
    this->data.i_visible_height = p_src->data.i_visible_height;
}

void MxVideoFormat::scaleCropAr(const MxVideoFormat *p_src )
{
    this->data.i_x_offset       = (uint64_t)p_src->data.i_x_offset       * this->data.i_width  / this->data.i_width;
    this->data.i_y_offset       = (uint64_t)p_src->data.i_y_offset       * this->data.i_height / this->data.i_height;
    this->data.i_visible_width  = (uint64_t)p_src->data.i_visible_width  * this->data.i_width  / this->data.i_width;
    this->data.i_visible_height = (uint64_t)p_src->data.i_visible_height * this->data.i_height / this->data.i_height;

    this->data.i_sar_num *= p_src->i_width;
    this->data.i_sar_den *= p_dst->i_width;
    mxUreduce(&this->data.i_sar_num, &this->data.i_sar_den,
                this->data.i_sar_num, this->data.i_sar_den, 65536);

    this->data.i_sar_num *= this->data.i_height;
    this->data.i_sar_den *= p_src->data.i_height;
    mxUreduce(&this->data.i_sar_num, &this->data.i_sar_den,
                this->data.i_sar_num, this->data.i_sar_den, 65536);
}

video_transform_t MxVideoFormat::getTransform( video_orientation_t src,
                                             video_orientation_t dst )
{
    unsigned angle1, angle2;
    bool hflip1, hflip2;

    transform_GetBasicOps(  (video_transform_t)src, &angle1, &hflip1 );
    transform_GetBasicOps( transform_Inverse( (video_transform_t)dst ),
                           &angle2, &hflip2 );

    int angle = (angle1 + angle2) % 360;
    bool hflip = hflip1 ^ hflip2;

    return transform_FromBasicOps(angle, hflip);
}

void MxVideoFormat::transformBy(video_transform_t transform )
{
    /* Get destination orientation */
    unsigned angle1, angle2;
    bool hflip1, hflip2;

    transform_GetBasicOps( transform, &angle1, &hflip1 );
    transform_GetBasicOps( (video_transform_t)this->data.orientation, &angle2, &hflip2 );

    unsigned angle = (angle2 - angle1 + 360) % 360;
    bool hflip = hflip2 ^ hflip1;

    video_orientation_t dst_orient = ORIENT_NORMAL;

    if( hflip ) {

        if( angle == 0 )
            dst_orient = ORIENT_HFLIPPED;
        else if( angle == 90 )
            dst_orient = ORIENT_ANTI_TRANSPOSED;
        else if( angle == 180 )
            dst_orient = ORIENT_VFLIPPED;
        else if( angle == 270 )
            dst_orient = ORIENT_TRANSPOSED;
    }
    else {

        if( angle == 90 )
            dst_orient = ORIENT_ROTATED_90;
        else if( angle == 180 )
            dst_orient = ORIENT_ROTATED_180;
        else if( angle == 270 )
            dst_orient = ORIENT_ROTATED_270;
    }

    /* Apply transform */
    if( ORIENT_IS_SWAP( this->data.orientation ) != ORIENT_IS_SWAP( dst_orient ) )
    {
        Data scratch = this->data;

        this->data.i_width = scratch.i_height;
        this->data.i_visible_width = scratch.i_visible_height;
        this->data.i_height = scratch.i_width;
        this->data.i_visible_height = scratch.i_visible_width;
        this->data.i_x_offset = scratch.i_y_offset;
        this->data.i_y_offset = scratch.i_x_offset;
        this->data.i_sar_num = scratch.i_sar_den;
        this->data.i_sar_den = scratch.i_sar_num;
    }

    this->data.orientation = dst_orient;
}

void MxVideoFormat::transformTo(video_orientation_t dst_orientation )
{
    video_transform_t transform = video_format_GetTransform(this->data.orientation, dst_orientation);
    video_format_TransformBy(transform);
}

void MxVideoFormat::applyRotation(const video_format_t * in )
{
    out->data = in->data;

    video_format_TransformTo(ORIENT_NORMAL);
}

bool MxVideoFormat::isSimilar( const MxVideoFormat *other)
{
    if( this->data.i_chroma != other->data.i_chroma )
        return false;

    if( this->data.i_width != other->data.i_width || this->data.i_height != other->data.i_height ||
        this->data.i_visible_width != other->data.i_visible_width ||
        this->data.i_visible_height != other->data.i_visible_height ||
        this->data.i_x_offset != other->data.i_x_offset || this->data.i_y_offset != other->data.i_y_offset )
        return false;
    if( (int64_t)this->data.i_sar_num * other->data.i_sar_den !=
        (int64_t)other->data.i_sar_num * this->data.i_sar_den )
        return false;

    if( this->data.orientation != other->data.orientation)
        return false;

    if( this->data.multiview_mode!= other->data.multiview_mode )
       return false;

    if( this->data.i_chroma == MX_CODEC_RGB15 ||
        this->data.i_chroma == MX_CODEC_RGB16 ||
        this->data.i_chroma == MX_CODEC_RGB24 ||
        this->data.i_chroma == MX_CODEC_RGB32 )
    {
        video_format_t v1 = *f1;
        video_format_t v2 = *f2;

        video_format_FixRgb( &v1 );
        video_format_FixRgb( &v2 );

        if( v1.i_rmask != v2.i_rmask ||
            v1.i_gmask != v2.i_gmask ||
            v1.i_bmask != v2.i_bmask )
            return false;
    }
    return true;
}

void MxVideoFormat::print( CMxObject *p_this, const char *psz_text)
{
    msg_Dbg( p_this,
             "%s sz %ix%i, of (%i,%i), vsz %ix%i, 4cc %4.4s, sar %i:%i, msk r0x%x g0x%x b0x%x",
             psz_text,
             this->data.i_width, this->data.i_height, this->data.i_x_offset, this->data.i_y_offset,
             this->data.i_visible_width, this->data.i_visible_height,
             (char*)&this->data.i_chroma,
             this->data.i_sar_num, fmt->i_sar_den,
             this->data.i_rmask, this->data.i_gmask, this->data.i_bmask );
}

