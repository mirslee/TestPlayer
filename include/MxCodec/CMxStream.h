#ifndef CMXSTREAM_H
#define CMXSTREAM_H 1

#include "MxSystem/CMxBlock.h"
#include "MxCore/vlc_input_item.h"
#include "CMxObject.h"
#include "MxSystem/MxCharSet.h"


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

struct module_t;
struct input_thread_t;
class CMxStream {
public:
    CMxStream();
    virtual ~CMxStream();
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
    
    virtual ssize_t read(void *buf, size_t len) = 0;
    virtual CMxBlock* readBlock(bool *eof);
    virtual int readdir(input_item_node_t *);
    virtual int seek(uint64_t);
    virtual void control(int i_query, va_list);
    
    
    void *p_sys;
    
    input_thread_t *p_input;
    
protected:
    CMxBlock *block;
    CMxBlock *peek;
    uint64_t offset;
    bool eof;
    
    /* UTF-16 and UTF-32 file reading */
    struct {
        MxIconv   conv;
        unsigned char char_width;
        bool          little_endian;
    } text;
    
private:
    ssize_t copyBlock(CMxBlock** ppBlock,void *buf, size_t len);
    ssize_t readRaw(void *buf, size_t len);
};

#endif
