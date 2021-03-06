/*****************************************************************************
 * es.h: Elementary stream formats descriptions
 *****************************************************************************

 *****************************************************************************/

#ifndef MXES_H
#define MXES_H 1

#include "MxError.h"
#include "MxTextStyle.h"

/**
 * \file
 * This file defines the elementary streams format types
 */

/**
 * video palette data
 * \see MxVideoFormat
 * \see MxSubsFormat
 */
#define VIDEO_PALETTE_COLORS_MAX 256

struct video_palette_t
{
    int i_entries;      /**< to keep the compatibility with libavcodec's palette */
    uint8_t palette[VIDEO_PALETTE_COLORS_MAX][4];  /**< 4-byte RGBA/YUVA palette */
};

/**
 * audio replay gain description
 */
#define AUDIO_REPLAY_GAIN_MAX (2)
#define AUDIO_REPLAY_GAIN_TRACK (0)
#define AUDIO_REPLAY_GAIN_ALBUM (1)
typedef struct
{
    /* true if we have the peak value */
    bool pb_peak[AUDIO_REPLAY_GAIN_MAX];
    /* peak value where 1.0 means full sample value */
    float      pf_peak[AUDIO_REPLAY_GAIN_MAX];
    
    /* true if we have the gain value */
    bool pb_gain[AUDIO_REPLAY_GAIN_MAX];
    /* gain value in dB */
    float      pf_gain[AUDIO_REPLAY_GAIN_MAX];
} audio_replay_gain_t;

/**
 * Audio channel type
 */
typedef enum audio_channel_type_t
{
    AUDIO_CHANNEL_TYPE_BITMAP,
    AUDIO_CHANNEL_TYPE_AMBISONICS,
} audio_channel_type_t;

/**
 * audio format description
 */
struct MxAudioFormat
{
    MxFourcc i_format;                          /**< audio format fourcc */
    unsigned int i_rate;                              /**< audio sample-rate */
    
    /* Describes the channels configuration of the samples (ie. number of
     * channels which are available in the buffer, and positions). */
    uint16_t     i_physical_channels;
    
    /* Describes the chan mode, either set from the input
     * (demux/codec/packetizer) or overridden by the user, used by audio
     * filters. */
    uint16_t     i_chan_mode;
    
    /* Channel type */
    audio_channel_type_t channel_type;
    
    /* Optional - for A/52, SPDIF and DTS types : */
    /* Bytes used by one compressed frame, depends on bitrate. */
    unsigned int i_bytes_per_frame;
    
    /* Number of sampleframes contained in one compressed frame. */
    unsigned int i_frame_length;
    /* Please note that it may be completely arbitrary - buffers are not
     * obliged to contain a integral number of so-called "frames". It's
     * just here for the division :
     * buffer_size = i_nb_samples * i_bytes_per_frame / i_frame_length
     */
    
    /* FIXME ? (used by the codecs) */
    unsigned     i_bitspersample;
    unsigned     i_blockalign;
    uint8_t      i_channels; /* must be <=32 */
};

/* Values available for audio channels */
#define AOUT_CHAN_CENTER            0x1
#define AOUT_CHAN_LEFT              0x2
#define AOUT_CHAN_RIGHT             0x4
#define AOUT_CHAN_REARCENTER        0x10
#define AOUT_CHAN_REARLEFT          0x20
#define AOUT_CHAN_REARRIGHT         0x40
#define AOUT_CHAN_MIDDLELEFT        0x100
#define AOUT_CHAN_MIDDLERIGHT       0x200
#define AOUT_CHAN_LFE               0x1000

#define AOUT_CHANS_FRONT  (AOUT_CHAN_LEFT       | AOUT_CHAN_RIGHT)
#define AOUT_CHANS_MIDDLE (AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT)
#define AOUT_CHANS_REAR   (AOUT_CHAN_REARLEFT   | AOUT_CHAN_REARRIGHT)
#define AOUT_CHANS_CENTER (AOUT_CHAN_CENTER     | AOUT_CHAN_REARCENTER)

#define AOUT_CHANS_STEREO AOUT_CHANS_2_0
#define AOUT_CHANS_2_0    (AOUT_CHANS_FRONT)
#define AOUT_CHANS_2_1    (AOUT_CHANS_FRONT | AOUT_CHAN_LFE)
#define AOUT_CHANS_3_0    (AOUT_CHANS_FRONT | AOUT_CHAN_CENTER)
#define AOUT_CHANS_3_1    (AOUT_CHANS_3_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_4_0    (AOUT_CHANS_FRONT | AOUT_CHANS_REAR)
#define AOUT_CHANS_4_1    (AOUT_CHANS_4_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_5_0    (AOUT_CHANS_4_0   | AOUT_CHAN_CENTER)
#define AOUT_CHANS_5_1    (AOUT_CHANS_5_0   | AOUT_CHAN_LFE)
#define AOUT_CHANS_6_0    (AOUT_CHANS_4_0   | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_7_0    (AOUT_CHANS_6_0   | AOUT_CHAN_CENTER)
#define AOUT_CHANS_7_1    (AOUT_CHANS_5_1   | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_8_1    (AOUT_CHANS_7_1   | AOUT_CHAN_REARCENTER)

#define AOUT_CHANS_4_0_MIDDLE (AOUT_CHANS_FRONT | AOUT_CHANS_MIDDLE)
#define AOUT_CHANS_4_CENTER_REAR (AOUT_CHANS_FRONT | AOUT_CHANS_CENTER)
#define AOUT_CHANS_5_0_MIDDLE (AOUT_CHANS_4_0_MIDDLE | AOUT_CHAN_CENTER)
#define AOUT_CHANS_6_1_MIDDLE (AOUT_CHANS_5_0_MIDDLE | AOUT_CHAN_REARCENTER | AOUT_CHAN_LFE)

/* Maximum number of mapped channels (or the maximum of bits set in
 * i_physical_channels) */
#define AOUT_CHAN_MAX               9

/* Maximum number of unmapped channels */
#define INPUT_CHAN_MAX              64

/* Values available for i_chan_mode only */
#define AOUT_CHANMODE_DUALMONO    0x1
#define AOUT_CHANMODE_DOLBYSTEREO 0x2

/**
 * Picture orientation.
 */
enum video_orientation_t
{
    ORIENT_TOP_LEFT = 0, /**< Top line represents top, left column left. */
    ORIENT_TOP_RIGHT, /**< Flipped horizontally */
    ORIENT_BOTTOM_LEFT, /**< Flipped vertically */
    ORIENT_BOTTOM_RIGHT, /**< Rotated 180 degrees */
    ORIENT_LEFT_TOP, /**< Transposed */
    ORIENT_LEFT_BOTTOM, /**< Rotated 90 degrees clockwise */
    ORIENT_RIGHT_TOP, /**< Rotated 90 degrees anti-clockwise */
    ORIENT_RIGHT_BOTTOM, /**< Anti-transposed */
    
    ORIENT_NORMAL      = ORIENT_TOP_LEFT,
    ORIENT_TRANSPOSED  = ORIENT_LEFT_TOP,
    ORIENT_ANTI_TRANSPOSED = ORIENT_RIGHT_BOTTOM,
    ORIENT_HFLIPPED    = ORIENT_TOP_RIGHT,
    ORIENT_VFLIPPED    = ORIENT_BOTTOM_LEFT,
    ORIENT_ROTATED_180 = ORIENT_BOTTOM_RIGHT,
    ORIENT_ROTATED_270 = ORIENT_LEFT_BOTTOM,
    ORIENT_ROTATED_90  = ORIENT_RIGHT_TOP,
};
/** Convert EXIF orientation to enum video_orientation_t */
#define ORIENT_FROM_EXIF(exif) ((0x57642310U >> (4 * ((exif) - 1))) & 7)
/** Convert enum video_orientation_t to EXIF */
#define ORIENT_TO_EXIF(orient) ((0x76853421U >> (4 * (orient))) & 15)
/** If the orientation is natural or mirrored */
#define ORIENT_IS_MIRROR(orient) parity(orient)
/** If the orientation swaps dimensions */
#define ORIENT_IS_SWAP(orient) (((orient) & 4) != 0)
/** Applies horizontal flip to an orientation */
#define ORIENT_HFLIP(orient) ((orient) ^ 1)
/** Applies vertical flip to an orientation */
#define ORIENT_VFLIP(orient) ((orient) ^ 2)
/** Applies horizontal flip to an orientation */
#define ORIENT_ROTATE_180(orient) ((orient) ^ 3)

enum video_transform_t
{
    TRANSFORM_IDENTITY       = ORIENT_NORMAL,
    TRANSFORM_HFLIP          = ORIENT_HFLIPPED,
    TRANSFORM_VFLIP          = ORIENT_VFLIPPED,
    TRANSFORM_R180           = ORIENT_ROTATED_180,
    TRANSFORM_R270           = ORIENT_ROTATED_270,
    TRANSFORM_R90            = ORIENT_ROTATED_90,
    TRANSFORM_TRANSPOSE      = ORIENT_TRANSPOSED,
    TRANSFORM_ANTI_TRANSPOSE = ORIENT_ANTI_TRANSPOSED
};

enum video_multiview_mode_t
{
    /* No stereoscopy: 2D picture. */
    MULTIVIEW_2D = 0,
    
    /* Side-by-side with left eye first. */
    MULTIVIEW_STEREO_SBS,
    
    /* Top-bottom with left eye first. */
    MULTIVIEW_STEREO_TB,
    
    /* Row sequential with left eye first. */
    MULTIVIEW_STEREO_ROW,
    
    /* Column sequential with left eye first. */
    MULTIVIEW_STEREO_COL,
    
    /* Frame sequential with left eye first. */
    MULTIVIEW_STEREO_FRAME,
    
    /* Checkerboard pattern with left eye first. */
    MULTIVIEW_STEREO_CHECKERBOARD,
};

/**
 * Video projection mode.
 */
enum video_projection_mode_t
{
    PROJECTION_MODE_RECTANGULAR = 0,
    PROJECTION_MODE_EQUIRECTANGULAR,
    
    PROJECTION_MODE_CUBEMAP_LAYOUT_STANDARD = 0x100,
};

/**
 * Video color primaries (a.k.a. chromacities)
 */
enum video_color_primaries_t
{
    COLOR_PRIMARIES_UNDEF,
    COLOR_PRIMARIES_BT601_525,
    COLOR_PRIMARIES_BT601_625,
    COLOR_PRIMARIES_BT709,
    COLOR_PRIMARIES_BT2020,
    COLOR_PRIMARIES_DCI_P3,
    COLOR_PRIMARIES_FCC1953,
#define COLOR_PRIMARIES_SRGB            COLOR_PRIMARIES_BT709
#define COLOR_PRIMARIES_SMTPE_170       COLOR_PRIMARIES_BT601_525
#define COLOR_PRIMARIES_SMTPE_240       COLOR_PRIMARIES_BT601_525 /* Only differs from 1e10-4 in white Y */
#define COLOR_PRIMARIES_SMTPE_RP145     COLOR_PRIMARIES_BT601_525
#define COLOR_PRIMARIES_EBU_3213        COLOR_PRIMARIES_BT601_625
#define COLOR_PRIMARIES_BT470_BG        COLOR_PRIMARIES_BT601_625
#define COLOR_PRIMARIES_BT470_M         COLOR_PRIMARIES_FCC1953
#define COLOR_PRIMARIES_MAX             COLOR_PRIMARIES_FCC1953
};

/**
 * Video transfer functions
 */
enum video_transfer_func_t
{
    TRANSFER_FUNC_UNDEF,
    TRANSFER_FUNC_LINEAR,
    TRANSFER_FUNC_SRGB /*< Gamma 2.2 */,
    TRANSFER_FUNC_BT470_BG,
    TRANSFER_FUNC_BT470_M,
    TRANSFER_FUNC_BT709,
    TRANSFER_FUNC_SMPTE_ST2084,
    TRANSFER_FUNC_SMPTE_240,
    TRANSFER_FUNC_HLG,
#define TRANSFER_FUNC_BT2020            TRANSFER_FUNC_BT709
#define TRANSFER_FUNC_SMPTE_170         TRANSFER_FUNC_BT709
#define TRANSFER_FUNC_SMPTE_274         TRANSFER_FUNC_BT709
#define TRANSFER_FUNC_SMPTE_293         TRANSFER_FUNC_BT709
#define TRANSFER_FUNC_SMPTE_296         TRANSFER_FUNC_BT709
#define TRANSFER_FUNC_ARIB_B67          TRANSFER_FUNC_HLG
#define TRANSFER_FUNC_MAX               TRANSFER_FUNC_HLG
};

/**
 * Video color space (i.e. YCbCr matrices)
 */
enum video_color_space_t
{
    COLOR_SPACE_UNDEF,
    COLOR_SPACE_BT601,
    COLOR_SPACE_BT709,
    COLOR_SPACE_BT2020,
#define COLOR_SPACE_SRGB      COLOR_SPACE_BT709
#define COLOR_SPACE_SMPTE_170 COLOR_SPACE_BT601
#define COLOR_SPACE_SMPTE_240 COLOR_SPACE_SMPTE_170
#define COLOR_SPACE_MAX       COLOR_SPACE_BT2020
};

/**
 * Video chroma location
 */
enum video_chroma_location_t
{
    CHROMA_LOCATION_UNDEF,
    CHROMA_LOCATION_LEFT,   /*< Most common in MPEG-2 Video, H.264/265 */
    CHROMA_LOCATION_CENTER, /*< Most common in MPEG-1 Video, JPEG */
    CHROMA_LOCATION_TOP_LEFT,
    CHROMA_LOCATION_TOP_CENTER,
    CHROMA_LOCATION_BOTTOM_LEFT,
    CHROMA_LOCATION_BOTTOM_CENTER,
#define CHROMA_LOCATION_MAX CHROMA_LOCATION_BOTTOM_CENTER
};


/**
 * video format description
 */
struct MXCODEC_API MxVideoFormat
{
    struct Data {
        MxFourcc i_chroma;                               /**< picture chroma */
        
        unsigned int i_width;                                 /**< picture width */
        unsigned int i_height;                               /**< picture height */
        unsigned int i_x_offset;               /**< start offset of visible area */
        unsigned int i_y_offset;               /**< start offset of visible area */
        unsigned int i_visible_width;                 /**< width of visible area */
        unsigned int i_visible_height;               /**< height of visible area */
        
        unsigned int i_bits_per_pixel;             /**< number of bits per pixel */
        
        unsigned int i_sar_num;                   /**< sample/pixel aspect ratio */
        unsigned int i_sar_den;
        
        unsigned int i_frame_rate;                     /**< frame rate numerator */
        unsigned int i_frame_rate_base;              /**< frame rate denominator */
        
        uint32_t i_rmask, i_gmask, i_bmask;      /**< color masks for RGB chroma */
        int i_rrshift, i_lrshift;
        int i_rgshift, i_lgshift;
        int i_rbshift, i_lbshift;
        video_palette_t *p_palette;              /**< video palette from demuxer */
        video_orientation_t orientation;                /**< picture orientation */
        video_color_primaries_t primaries;                  /**< color primaries */
        video_transfer_func_t transfer;                   /**< transfer function */
        video_color_space_t space;                        /**< YCbCr color space */
        bool b_color_range_full;                    /**< 0-255 instead of 16-235 */
        video_chroma_location_t chroma_location;      /**< YCbCr chroma location */
        
        video_multiview_mode_t multiview_mode;        /** Multiview mode, 2D, 3D */
        
        video_projection_mode_t projection_mode;            /**< projection mode */
        MxViewpoint pose;
        uint32_t i_cubemap_padding; /**< padding in pixels of the cube map faces */
        
        struct {
            /* similar to SMPTE ST 2086 mastering display color volume */
            uint16_t primaries[3*2]; /* G,B,R / x,y */
            uint16_t white_point[2]; /* x,y */
            uint32_t max_luminance;
            uint32_t min_luminance;
        } mastering;
        struct {
            /* similar to CTA-861.3 content light level */
            uint16_t MaxCLL;  /* max content light level */
            uint16_t MaxFALL; /* max frame average light level */
        } lighting;
    };
    
    Data data;
    
public:
    MxVideoFormat() {
        memset(&data, 0, sizeof(Data));
    }
    MxVideoFormat(MxFourcc i_chroma) {
        MxVideoFormat();
        data.i_chroma = i_chroma;
    }
    int copy(const MxVideoFormat *p_src )
    {
        memcpy( &data, &p_src->data, sizeof( Data ) );
        if( p_src->data.p_palette )
        {
            data.p_palette = (video_palette_t *) malloc( sizeof( video_palette_t ) );
            if( !data.p_palette )
                return MX_ENOMEM;
            memcpy( &data.p_palette, p_src->data.p_palette, sizeof( video_palette_t ) );
        }
        return MX_SUCCESS;
    }
    void adjustColorSpace()
    {
        if ( data.primaries == COLOR_PRIMARIES_UNDEF )
        {
            if ( data.i_visible_height > 576 ) // HD
                data.primaries = COLOR_PRIMARIES_BT709;
            else if ( data.i_visible_height > 525 ) // PAL
                data.primaries = COLOR_PRIMARIES_BT601_625;
            else
                data.primaries = COLOR_PRIMARIES_BT601_525;
        }
        
        if ( data.transfer == TRANSFER_FUNC_UNDEF )
        {
            if ( data.i_visible_height > 576 ) // HD
                data.transfer = TRANSFER_FUNC_BT709;
            else
                data.transfer = TRANSFER_FUNC_SRGB;
        }
        
        if ( data.space == COLOR_SPACE_UNDEF )
        {
            if ( data.i_visible_height > 576 ) // HD
                data.space = COLOR_SPACE_BT709;
            else
                data.space = COLOR_SPACE_BT601;
        }
    }
    
    void clean() {
        free( data.p_palette );
        memset( &data, 0, sizeof( Data ) );
    }
    
    /**
     * It will fill up a MxVideoFormat using the given arguments.
     * Note that the MxVideoFormat must already be initialized.
     */
    void setup(MxFourcc i_chroma,
                                    int i_width, int i_height, int i_visible_width, int i_visible_height,
                                    int i_sar_num, int i_sar_den );
    
    /**
     * It will copy the crop properties from a MxVideoFormat to another.
     */
    void copyCrop(const MxVideoFormat * );
    
    /**
     * It will compute the crop/ar properties when scaling.
     */
    void scaleCropAr(const MxVideoFormat * );
    
    /**
     * This function "normalizes" the formats orientation, by switching the a/r according to the orientation,
     * producing a format whose orientation is ORIENT_NORMAL. It makes a shallow copy (pallette is not alloc'ed).
     */
    void applyRotation(const MxVideoFormat *in);
    
    /**
     * This function applies the transform operation to fmt.
     */
    void transformBy(video_transform_t transform);
    
    /**
     * This function applies the transforms necessary to fmt so that the resulting fmt
     * has the dst_orientation.
     */
    void transformTo(video_orientation_t dst_orientation);
    
    /**
     * Returns the operation required to transform src into dst.
     */
    video_transform_t getTransform(video_orientation_t src, video_orientation_t dst);
    
    /**
     * This function will check if the first video format is similar
     * to the second one.
     */
    bool isSimilar( const MxVideoFormat * );
    
    /**
     * It prints details about the given MxVideoFormat
     */
    void print( CMxObject *, const char *);
    
    /**
     * This function will fill all RGB shift from RGB masks.
     */
    void fixRgb();
};

/**
 * subtitles format description
 */
struct MxSubsFormat
{
    /* the character encoding of the text of the subtitle.
     * all gettext recognized shorts can be used */
    char *psz_encoding;
    
    
    int  i_x_origin; /**< x coordinate of the subtitle. 0 = left */
    int  i_y_origin; /**< y coordinate of the subtitle. 0 = top */
    
    struct
    {
        /*  */
        uint32_t palette[16+1];
        
        /* the width of the original movie the spu was extracted from */
        int i_original_frame_width;
        /* the height of the original movie the spu was extracted from */
        int i_original_frame_height;
    } spu;
    
    struct
    {
        int i_id;
    } dvb;
    struct
    {
        int i_magazine;
        int i_page;
    } teletext;
    struct
    {
        uint8_t i_channel;
        /* Reorder depth of transport video, -1 for no reordering */
        int i_reorder_depth;
    } cc;
    
    text_style_t *p_style; /* Default styles to use */
};

#define SPU_PALETTE_DEFINED  0xbeefbeef

/**
 * ES language definition
 */
typedef struct extra_languages_t
{
    char *psz_language;
    char *psz_description;
} extra_languages_t;

/** ES Categories */
enum es_format_category_e
{
    UNKNOWN_ES = 0x00,
    VIDEO_ES,
    AUDIO_ES,
    SPU_ES,
    DATA_ES,
};
#define ES_CATEGORY_COUNT (DATA_ES + 1)

/**
 * ES format definition
 */
#define ES_PRIORITY_NOT_SELECTABLE  -2
#define ES_PRIORITY_NOT_DEFAULTABLE -1
#define ES_PRIORITY_SELECTABLE_MIN   0
#define ES_PRIORITY_MIN ES_PRIORITY_NOT_SELECTABLE
struct MXCODEC_API MxEsFormat
{
    struct Data {
        enum es_format_category_e i_cat;    /**< ES category */
        MxFourcc    i_codec;            /**< FOURCC */
        MxFourcc    i_original_fourcc;  /**< original FOURCC from the container */
        
        int             i_id;       /**< es identifier, where means
                                     -1: let the core mark the right id
                                     >=0: valid id */
        int             i_group;    /**< group identifier, where means:
                                     -1 : standalone
                                     >= 0 then a "group" (program) is created
                                     for each value */
        int             i_priority; /**< priority, where means:
                                     -2 : mean not selectable by the users
                                     -1 : mean not selected by default even
                                     when no other stream
                                     >=0: priority */
        
        char            *psz_language;        /**< human-readable language name */
        char            *psz_description;     /**< human-readable description of language */
        unsigned        i_extra_languages;    /**< length in bytes of extra language data pointer */
        extra_languages_t *p_extra_languages; /**< extra language data needed by some decoders */
        
        union {
            struct {
                MxAudioFormat  audio;    /**< description of audio format */
                audio_replay_gain_t audio_replay_gain; /*< audio replay gain information */
            };
            MxVideoFormat video;     /**< description of video format */
            MxSubsFormat  subs;      /**< description of subtitle format */
        };
        
        unsigned int   i_bitrate; /**< bitrate of this ES */
        int      i_profile;       /**< codec specific information (like real audio flavor, mpeg audio layer, h264 profile ...) */
        int      i_level;         /**< codec specific information: indicates maximum restrictions on the stream (resolution, bitrate, codec features ...) */
        
        bool     b_packetized;  /**< whether the data is packetized (ie. not truncated) */
        int     i_extra;        /**< length in bytes of extra data pointer */
        void    *p_extra;       /**< extra data needed by some decoders or muxers */
    };
    
    Data data;
    
    /**
     * This function will initialize a MxEsFormat structure.
     */
    void init(int i_cat, MxFourcc i_codec );
    
    /**
     * This function will initialize a MxEsFormat structure from a MxVideoFormat.
     */
    void initFromVideo(const MxVideoFormat * );
    
    /**
     * This functions will copy a MxEsFormat.
     */
    int copy(const MxEsFormat *p_src );
    
    /**
     * This function will clean up a MxEsFormat and release all associated
     * resources.
     * You can call it multiple times on the same structure.
     */
    void clean();
    
    /**
     * This function will check if the first ES format is similar
     * to the second one.
     *
     * All descriptive fields are ignored.
     */
    bool isSimilar(const MxEsFormat * );
    
    /**
     * Changes ES format to another category
     * Format must have been properly initialized
     */
    void change( int i_cat, MxFourcc i_codec );
};

static inline video_transform_t transform_Inverse( video_transform_t transform )
{
    switch ( transform ) {
        case TRANSFORM_R90:
            return TRANSFORM_R270;
        case TRANSFORM_R270:
            return TRANSFORM_R90;
        default:
            return transform;
    }
}

#endif
