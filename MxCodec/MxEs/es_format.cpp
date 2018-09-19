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

/* */
void video_format_FixRgb( video_format_t *p_fmt )
{
    /* FIXME find right default mask */
    if( !p_fmt->i_rmask || !p_fmt->i_gmask || !p_fmt->i_bmask )
    {
        switch( p_fmt->i_chroma )
        {
        case VLC_CODEC_RGB15:
            p_fmt->i_rmask = 0x7c00;
            p_fmt->i_gmask = 0x03e0;
            p_fmt->i_bmask = 0x001f;
            break;

        case VLC_CODEC_RGB16:
            p_fmt->i_rmask = 0xf800;
            p_fmt->i_gmask = 0x07e0;
            p_fmt->i_bmask = 0x001f;
            break;

        case VLC_CODEC_RGB24:
            p_fmt->i_rmask = 0xff0000;
            p_fmt->i_gmask = 0x00ff00;
            p_fmt->i_bmask = 0x0000ff;
            break;
        case VLC_CODEC_RGB32:
            p_fmt->i_rmask = 0x00ff0000;
            p_fmt->i_gmask = 0x0000ff00;
            p_fmt->i_bmask = 0x000000ff;
            break;

        default:
            return;
        }
    }

    MaskToShift( &p_fmt->i_lrshift, &p_fmt->i_rrshift,
                 p_fmt->i_rmask );
    MaskToShift( &p_fmt->i_lgshift, &p_fmt->i_rgshift,
                 p_fmt->i_gmask );
    MaskToShift( &p_fmt->i_lbshift, &p_fmt->i_rbshift,
                 p_fmt->i_bmask );
}

void MxVideoFormat::video_format_Setup(MxFourcc i_chroma,
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

void MxVideoFormat::video_format_CopyCrop(const MxVideoFormat *p_src )
{
    this->data.i_x_offset       = p_src->data.i_x_offset;
    this->data.i_y_offset       = p_src->data.i_y_offset;
    this->data.i_visible_width  = p_src->data.i_visible_width;
    this->data.i_visible_height = p_src->data.i_visible_height;
}

void MxVideoFormat::video_format_ScaleCropAr(const MxVideoFormat *p_src )
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

video_transform_t MxVideoFormat::video_format_GetTransform( video_orientation_t src,
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

void MxVideoFormat::video_format_TransformBy(video_transform_t transform )
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

void MxVideoFormat::video_format_TransformTo(video_orientation_t dst_orientation )
{
    video_transform_t transform = video_format_GetTransform(this->data.orientation, dst_orientation);
    video_format_TransformBy(transform);
}

void MxVideoFormat::video_format_ApplyRotation(const video_format_t * in )
{
    out->data = in->data;

    video_format_TransformTo(ORIENT_NORMAL);
}

bool MxVideoFormat::video_format_IsSimilar( const MxVideoFormat *other)
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
void MxVideoFormat::video_format_Print( CMxObject *p_this, const char *psz_text)
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

void es_format_Init( es_format_t *fmt,
                     int i_cat, vlc_fourcc_t i_codec )
{
    memset(fmt, 0, sizeof (*fmt));
    fmt->i_cat                  = i_cat;
    fmt->i_codec                = i_codec;
    fmt->i_profile              = -1;
    fmt->i_level                = -1;
    fmt->i_id                   = -1;
    fmt->i_priority             = ES_PRIORITY_SELECTABLE_MIN;
    fmt->psz_language           = NULL;
    fmt->psz_description        = NULL;
    fmt->p_extra_languages      = NULL;

    if (fmt->i_cat == VIDEO_ES)
        video_format_Init(&fmt->video, 0);

    fmt->b_packetized           = true;
    fmt->p_extra                = NULL;
}

void es_format_InitFromVideo( es_format_t *p_es, const video_format_t *p_fmt )
{
    es_format_Init( p_es, VIDEO_ES, p_fmt->i_chroma );
    video_format_Copy( &p_es->video, p_fmt );
}

int es_format_Copy(es_format_t *restrict dst, const es_format_t *src)
{
    int ret = VLC_SUCCESS;

    *dst = *src;

    if (src->psz_language != NULL)
    {
        dst->psz_language = strdup(src->psz_language);
        if (unlikely(dst->psz_language == NULL))
            ret = VLC_ENOMEM;
    }
    if (src->psz_description != NULL)
    {
        dst->psz_description = strdup(src->psz_description);
        if (unlikely(dst->psz_description == NULL))
            ret = VLC_ENOMEM;
    }

    if (src->i_extra > 0)
    {
        assert(src->p_extra != NULL);
        dst->p_extra = malloc( src->i_extra );

        if( likely(dst->p_extra != NULL) )
            memcpy(dst->p_extra, src->p_extra, src->i_extra);
        else
        {
            dst->i_extra = 0;
            ret = VLC_ENOMEM;
        }
    }
    else
        dst->p_extra = NULL;

    if (src->i_cat == VIDEO_ES)
        ret = video_format_Copy( &dst->video, &src->video );

    if (src->i_cat == SPU_ES)
    {
        if (src->subs.psz_encoding != NULL)
        {
            dst->subs.psz_encoding = strdup(src->subs.psz_encoding);
            if (unlikely(dst->subs.psz_encoding == NULL))
                ret = VLC_ENOMEM;
        }
        if (src->subs.p_style != NULL)
        {
            dst->subs.p_style = text_style_Duplicate(src->subs.p_style);
            if (unlikely(dst->subs.p_style == NULL))
                ret = VLC_ENOMEM;
        }
    }

    if (src->i_extra_languages > 0)
    {
        assert(src->p_extra_languages != NULL);
        dst->p_extra_languages = calloc(dst->i_extra_languages,
                                        sizeof (*dst->p_extra_languages));
        if (likely(dst->p_extra_languages != NULL))
        {
            for (unsigned i = 0; i < dst->i_extra_languages; i++)
            {
                if (src->p_extra_languages[i].psz_language != NULL)
                    dst->p_extra_languages[i].psz_language = strdup(src->p_extra_languages[i].psz_language);
                if (src->p_extra_languages[i].psz_description != NULL)
                    dst->p_extra_languages[i].psz_description = strdup(src->p_extra_languages[i].psz_description);
            }
            dst->i_extra_languages = src->i_extra_languages;
        }
        else
        {
            dst->i_extra_languages = 0;
            ret = VLC_ENOMEM;
        }
    }
    return ret;
}

void es_format_Clean(es_format_t *fmt)
{
    free(fmt->psz_language);
    free(fmt->psz_description);
    assert(fmt->i_extra == 0 || fmt->p_extra != NULL);
    free(fmt->p_extra);

    if (fmt->i_cat == VIDEO_ES)
        video_format_Clean( &fmt->video );
    if (fmt->i_cat == SPU_ES)
    {
        free(fmt->subs.psz_encoding);

        if (fmt->subs.p_style != NULL)
            text_style_Delete(fmt->subs.p_style);
    }

    for (unsigned i = 0; i < fmt->i_extra_languages; i++)
    {
        free(fmt->p_extra_languages[i].psz_language);
        free(fmt->p_extra_languages[i].psz_description);
    }
    free(fmt->p_extra_languages);

    /* es_format_Clean can be called multiple times */
    es_format_Init(fmt, UNKNOWN_ES, 0);
}

bool es_format_IsSimilar( const es_format_t *p_fmt1, const es_format_t *p_fmt2 )
{
    if( p_fmt1->i_cat != p_fmt2->i_cat ||
        vlc_fourcc_GetCodec( p_fmt1->i_cat, p_fmt1->i_codec ) !=
        vlc_fourcc_GetCodec( p_fmt2->i_cat, p_fmt2->i_codec ) )
        return false;

    switch( p_fmt1->i_cat )
    {
    case AUDIO_ES:
    {
        audio_format_t a1 = p_fmt1->audio;
        audio_format_t a2 = p_fmt2->audio;

        if( a1.i_format && a2.i_format && a1.i_format != a2.i_format )
            return false;
        if( a1.channel_type != a2.channel_type ||
            a1.i_rate != a2.i_rate ||
            a1.i_channels != a2.i_channels ||
            a1.i_physical_channels != a2.i_physical_channels ||
            a1.i_chan_mode != a2.i_chan_mode )
            return false;
        if( p_fmt1->i_profile != p_fmt2->i_profile )
            return false;
        return true;
    }

    case VIDEO_ES:
    {
        video_format_t v1 = p_fmt1->video;
        video_format_t v2 = p_fmt2->video;
        if( !v1.i_chroma )
            v1.i_chroma = vlc_fourcc_GetCodec( p_fmt1->i_cat, p_fmt1->i_codec );
        if( !v2.i_chroma )
            v2.i_chroma = vlc_fourcc_GetCodec( p_fmt2->i_cat, p_fmt2->i_codec );
        return video_format_IsSimilar( &p_fmt1->video, &p_fmt2->video );
    }

    case SPU_ES:
    default:
        return true;
    }
}

