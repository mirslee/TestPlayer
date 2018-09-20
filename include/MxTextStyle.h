#ifndef MXTEXTSTYLE_H
#define MXTEXTSTYLE_H

/* Line breaking */
enum wrap_info_t
{
    STYLE_WRAP_DEFAULT = 0,   /**< Breaks on whitespace or fallback on char */
    STYLE_WRAP_CHAR,          /**< Breaks at character level only */
    STYLE_WRAP_NONE,          /**< No line breaks (except explicit ones) */
};

/**
 * Text style
 *
 * A text style is used to specify the formatting of text.
 * A font renderer can use the supplied information to render the
 * text specified.
 */
typedef struct
{
    /* Family font names */
    char *     psz_fontname;      /**< The name of the font */
    char *     psz_monofontname;  /**< The name of the mono font */
    
    uint16_t   i_features;        /**< Feature flags (means non default) */
    uint16_t   i_style_flags;     /**< Formatting style flags */
    
    /* Font style */
    float      f_font_relsize;    /**< The font size in video height % */
    int        i_font_size;       /**< The font size in pixels */
    int        i_font_color;      /**< The color of the text 0xRRGGBB
                                   (native endianness) */
    uint8_t    i_font_alpha;      /**< The transparency of the text.*/
    int        i_spacing;         /**< The spaceing between glyphs in pixels */
    
    /* Outline */
    int        i_outline_color;   /**< The color of the outline 0xRRGGBB */
    uint8_t    i_outline_alpha;   /**< The transparency of the outline */
    int        i_outline_width;   /**< The width of the outline in pixels */
    
    /* Shadow */
    int        i_shadow_color;    /**< The color of the shadow 0xRRGGBB */
    uint8_t    i_shadow_alpha;    /**< The transparency of the shadow. */
    int        i_shadow_width;    /**< The width of the shadow in pixels */
    
    /* Background (and karaoke) */
    int        i_background_color;/**< The color of the background 0xRRGGBB */
    uint8_t    i_background_alpha;/**< The transparency of the background */
    int        i_karaoke_background_color;/**< Background color for karaoke 0xRRGGBB */
    uint8_t    i_karaoke_background_alpha;/**< The transparency of the karaoke bg */
    
    wrap_info_t e_wrapinfo;
} text_style_t;

#define STYLE_ALPHA_OPAQUE      0xFF
#define STYLE_ALPHA_TRANSPARENT 0x00

/* Features flags for \ref i_features */
#define STYLE_NO_DEFAULTS               0x0
#define STYLE_FULLY_SET                 0xFFFF
#define STYLE_HAS_FONT_COLOR            (1 << 0)
#define STYLE_HAS_FONT_ALPHA            (1 << 1)
#define STYLE_HAS_FLAGS                 (1 << 2)
#define STYLE_HAS_OUTLINE_COLOR         (1 << 3)
#define STYLE_HAS_OUTLINE_ALPHA         (1 << 4)
#define STYLE_HAS_SHADOW_COLOR          (1 << 5)
#define STYLE_HAS_SHADOW_ALPHA          (1 << 6)
#define STYLE_HAS_BACKGROUND_COLOR      (1 << 7)
#define STYLE_HAS_BACKGROUND_ALPHA      (1 << 8)
#define STYLE_HAS_K_BACKGROUND_COLOR    (1 << 9)
#define STYLE_HAS_K_BACKGROUND_ALPHA    (1 << 10)
#define STYLE_HAS_WRAP_INFO             (1 << 11)

/* Style flags for \ref text_style_t */
#define STYLE_BOLD              (1 << 0)
#define STYLE_ITALIC            (1 << 1)
#define STYLE_OUTLINE           (1 << 2)
#define STYLE_SHADOW            (1 << 3)
#define STYLE_BACKGROUND        (1 << 4)
#define STYLE_UNDERLINE         (1 << 5)
#define STYLE_STRIKEOUT         (1 << 6)
#define STYLE_HALFWIDTH         (1 << 7)
#define STYLE_MONOSPACED        (1 << 8)
#define STYLE_DOUBLEWIDTH       (1 << 9)
#define STYLE_BLINK_FOREGROUND  (1 << 10)
#define STYLE_BLINK_BACKGROUND  (1 << 11)

#define STYLE_DEFAULT_FONT_SIZE 20
#define STYLE_DEFAULT_REL_FONT_SIZE 6.25


typedef struct text_segment_t text_segment_t;
/**
 * Text segment for subtitles
 *
 * This structure is used to store a formatted text, with mixed styles
 * Every segment is comprised of one text and a unique style
 *
 * On style change, a new segment is created with the next part of text
 * and the new style, and chained to the list
 *
 * Create with text_segment_New and clean the chain with
 * text_segment_ChainDelete
 */
struct text_segment_t {
    char *psz_text;                   /**< text string of the segment */
    text_style_t *style;              /**< style applied to this segment */
    text_segment_t *p_next;           /**< next segment */
};

/**
 * Create a default text style
 */
MXCORE_API text_style_t * text_style_New( void );

/**
 * Create a text style
 *
 * Set feature flags as argument if you want to set style defaults
 */
MXCORE_API text_style_t * text_style_Create( int );

/**
 * Copy a text style into another
 */
MXCORE_API text_style_t * text_style_Copy( text_style_t *, const text_style_t * );

/**
 * Duplicate a text style
 */
MXCORE_API text_style_t * text_style_Duplicate( const text_style_t * );

/**
 * Merge two styles using non default values
 *
 * Set b_override to true if you also want to overwrite non-defaults
 */
MXCORE_API void text_style_Merge( text_style_t *, const text_style_t *, bool b_override );

/**
 * Delete a text style created by text_style_New or text_style_Duplicate
 */
MXCORE_API void text_style_Delete( text_style_t * );

/**
 * This function will create a new text segment.
 *
 * You should use text_segment_ChainDelete to destroy it, to clean all
 * the linked segments, or text_segment_Delete to free a specic one
 *
 * This duplicates the string passed as argument
 */
MXCORE_API text_segment_t *text_segment_New( const char * );

/**
 * This function will create a new text segment and duplicates the style passed as argument
 *
 * You should use text_segment_ChainDelete to destroy it, to clean all
 * the linked segments, or text_segment_Delete to free a specic one
 *
 * This doesn't initialize the text.
 */
MXCORE_API text_segment_t *text_segment_NewInheritStyle( const text_style_t* p_style );

/**
 * Delete a text segment and its content.
 *
 * This assumes the segment is not part of a chain
 */
MXCORE_API void text_segment_Delete( text_segment_t * );

/**
 * This function will destroy a list of text segments allocated
 * by text_segment_New.
 *
 * You may pass it NULL.
 */
MXCORE_API void text_segment_ChainDelete( text_segment_t * );

/**
 * This function will copy a text_segment and its chain into a new one
 *
 * You may give it NULL, but it will return NULL.
 */
MXCORE_API text_segment_t * text_segment_Copy( text_segment_t * );

/**
 * Returns an integer representation of an HTML color.
 *
 * @param psz_value An HTML color, which can be either:
 *  - A standard HTML color (red, cyan, ...) as defined in p_html_colors
 *  - An hexadecimal color, of the form [#][AA]RRGGBB
 * @param ok If non-null, true will be stored in this pointer to signal
 *           a successful conversion
 */
MXCORE_API unsigned int mxHtmlColor( const char *psz_value, bool* ok );


#endif /* MXTEXTSTYLE_H */
