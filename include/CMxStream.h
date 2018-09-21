#ifndef CMXSTREAM_H
#define CMXSTREAM_H 1

#include "CMxBlock.h"
#include "vlc_input_item.h"
#include "CMxObject.h"


/**
 * Possible commands
 */
enum stream_query_e
{
    /* capabilities */
    STREAM_CAN_SEEK,            /**< arg1= bool *   res=cannot fail*/
    STREAM_CAN_FASTSEEK,        /**< arg1= bool *   res=cannot fail*/
    STREAM_CAN_PAUSE,           /**< arg1= bool *   res=cannot fail*/
    STREAM_CAN_CONTROL_PACE,    /**< arg1= bool *   res=cannot fail*/
    /* */
    STREAM_GET_SIZE=6,          /**< arg1= uint64_t *     res=can fail */
    STREAM_IS_DIRECTORY,        /**< res=can fail */
    
    /* */
    STREAM_GET_PTS_DELAY = 0x101,/**< arg1= int64_t* res=cannot fail */
    STREAM_GET_TITLE_INFO, /**< arg1=input_title_t*** arg2=int* res=can fail */
    STREAM_GET_TITLE,       /**< arg1=unsigned * res=can fail */
    STREAM_GET_SEEKPOINT,   /**< arg1=unsigned * res=can fail */
    STREAM_GET_META,        /**< arg1= vlc_meta_t *       res=can fail */
    STREAM_GET_CONTENT_TYPE,    /**< arg1= char **         res=can fail */
    STREAM_GET_SIGNAL,      /**< arg1=double *pf_quality, arg2=double *pf_strength   res=can fail */
    STREAM_GET_TAGS,        /**< arg1=const block_t ** res=can fail */
    
    STREAM_SET_PAUSE_STATE = 0x200, /**< arg1= bool        res=can fail */
    STREAM_SET_TITLE,       /**< arg1= int          res=can fail */
    STREAM_SET_SEEKPOINT,   /**< arg1= int          res=can fail */
    
    /* XXX only data read through vlc_stream_Read/Block will be recorded */
    STREAM_SET_RECORD_STATE,     /**< arg1=bool, arg2=const char *psz_ext (if arg1 is true)  res=can fail */
    
    STREAM_SET_PRIVATE_ID_STATE = 0x1000, /* arg1= int i_private_data, bool b_selected    res=can fail */
    STREAM_SET_PRIVATE_ID_CA,             /* arg1= int i_program_number, uint16_t i_vpid, uint16_t i_apid1, uint16_t i_apid2, uint16_t i_apid3, uint8_t i_length, uint8_t *p_data */
    STREAM_GET_PRIVATE_ID_STATE,          /* arg1=int i_private_data arg2=bool *          res=can fail */
};

class CMxStream {
public:
    ssize_t vlc_stream_Read(void *buf, size_t len);
    ssize_t vlc_stream_ReadPartial(void *buf, size_t len);
    ssize_t vlc_stream_Peek(const uint8_t **, size_t);
    CMxBlock *vlc_stream_ReadBlock();
    uint64_t vlc_stream_Tell();
    bool vlc_stream_Eof();
    int vlc_stream_Seek(uint64_t offset);
    int vlc_stream_vaControl(int query, va_list args);
    CMxBlock *vlc_stream_Block(size_t);
    char *vlc_stream_ReadLine();
    int vlc_stream_ReadDir(input_item_node_t *);
    void vlc_stream_Delete();
    CMxStream *vlc_stream_CommonNew(CMxObject *, void (*)(CMxStream *));
    CMxStream *vlc_stream_MemoryNew(CMxObject *obj, uint8_t *base,size_t size, bool preserve);
    CMxStream * vlc_stream_NewURL(CMxObject *obj, const char *url);
    CMxStream *vlc_stream_fifo_New(CMxObject *parent);
    int vlc_stream_fifo_Queue(CMxStream *s, CMxBlock *block);
    ssize_t vlc_stream_fifo_Write(const void *buf,size_t len);
    void vlc_stream_fifo_Close();
    CMxStream* vlc_stream_FilterNew( CMxStream *p_source, const char *psz_stream_filter );
    int vlc_stream_FilterDefaultReadDir(input_item_node_t *p_node);
    
public:
    MX_COMMON_MEMBERS
    module_t    *p_module;
    
    char        *psz_name;
    char        *psz_url; /**< Full URL or MRL (can be NULL) */
    const char  *psz_location; /**< Location (URL with the scheme stripped) */
    char        *psz_filepath; /**< Local file path (if applicable) */
    bool         b_preparsing; /**< True if this access is used to preparse */
    
    /* Stream source for stream filter */
    CMxStream *p_source;
    ssize_t     (*pf_read)(stream_t *, void *buf, size_t len);
    
    CMxBlock    *(*pf_block)(stream_t *, bool *eof);
    
    int         (*pf_readdir)(stream_t *, input_item_node_t *);
    
    int         (*pf_seek)(stream_t *, uint64_t);
    
    int         (*pf_control)(stream_t *, int i_query, va_list);
    
    void *p_sys;
    
    input_thread_t *p_input;
};

    /**
     * \defgroup stream Stream
     * \ingroup input
     * Buffered input byte streams
     * @{
     * \file
     * Byte streams and byte stream filter modules interface
     */
    
    /**
     * stream_t definition
     */
    
    struct stream_t
    {
        MX_COMMON_MEMBERS
        
        /* Module properties for stream filter */
        module_t    *p_module;
        
        char        *psz_name;
        char        *psz_url; /**< Full URL or MRL (can be NULL) */
        const char  *psz_location; /**< Location (URL with the scheme stripped) */
        char        *psz_filepath; /**< Local file path (if applicable) */
        bool         b_preparsing; /**< True if this access is used to preparse */
        
        /* Stream source for stream filter */
        stream_t *p_source;
        ssize_t     (*pf_read)(stream_t *, void *buf, size_t len);

        block_t    *(*pf_block)(stream_t *, bool *eof);

        int         (*pf_readdir)(stream_t *, input_item_node_t *);

        int         (*pf_seek)(stream_t *, uint64_t);

        int         (*pf_control)(stream_t *, int i_query, va_list);

        void *p_sys;
        
        /* Weak link to parent input */
        input_thread_t *p_input;
    };
    

    


    
    static inline int vlc_stream_Control(stream_t *s, int query, ...)
    {
        va_list ap;
        int ret;
        
        va_start(ap, query);
        ret = vlc_stream_vaControl(s, query, ap);
        va_end(ap);
        return ret;
    }

/**
 * Get the size of the stream.
 */
VLC_USED static inline int vlc_stream_GetSize( stream_t *s, uint64_t *size )
{
    return vlc_stream_Control( s, STREAM_GET_SIZE, size );
}
    
    static inline int64_t stream_Size( stream_t *s )
    {
        uint64_t i_pos;
        
        if( vlc_stream_GetSize( s, &i_pos ) )
            return 0;
        if( i_pos >> 62 )
            return (int64_t)1 << 62;
        return i_pos;
    }
    
    VLC_USED
    static inline bool stream_HasExtension( stream_t *s, const char *extension )
    {
        const char *name = (s->psz_filepath != NULL) ? s->psz_filepath
        : s->psz_url;
        const char *ext = strrchr( name, '.' );
        return ext != NULL && !strcasecmp( ext, extension );
    }
    
    /**
     * Get the Content-Type of a stream, or NULL if unknown.
     * Result must be free()'d.
     */
    static inline char *stream_ContentType( stream_t *s )
    {
        char *res;
        if( vlc_stream_Control( s, STREAM_GET_CONTENT_TYPE, &res ) )
            return NULL;
        return res;
    }

    VLC_USED
    static inline char *stream_MimeType( stream_t *s )
    {
        char* mime_type = stream_ContentType( s );
        
        if( mime_type ) /* strip parameters */
            mime_type[strcspn( mime_type, " ;" )] = '\0';
        
        return mime_type;
    }

    VLC_USED
    static inline bool stream_IsMimeType(stream_t *s, const char *type)
    {
        char *mime = stream_MimeType(s);
        if (mime == NULL)
            return false;
        
        bool ok = !strcasecmp(mime, type);
        free(mime);
        return ok;
    }


#define vlc_stream_MemoryNew(a, b, c, d) \
vlc_stream_MemoryNew(VLC_OBJECT(a), b, c, d)
    

#define vlc_stream_NewURL(a, b) vlc_stream_NewURL(VLC_OBJECT(a), b)
    


#define stream_FilterSetDefaultReadDir(stream) \
do { \
(stream)->pf_readdir = vlc_stream_FilterDefaultReadDir; \
} while (0)




#endif
