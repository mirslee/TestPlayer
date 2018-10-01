#include "stdafx.h"
#include "CMxStream.h"
#include "MxSystem/MxMessages.h"
#include "MxError.h"
#include "MxCommon.h"
#include <assert.h>
#include <limits.h>

#ifdef _WIN32
#define SSIZE_MAX LONG_MAX
#endif

CMxStream::CMxStream() {
    p_module = NULL;
    psz_url = NULL;
    p_source = NULL;
    p_sys = NULL;
    p_input = NULL;
    block = NULL;
    peek = NULL;
    offset = 0;
    eof = false;
    
    /* UTF16 and UTF32 text file conversion */
    this->text.conv = (MxIconv)(-1);
    this->text.char_width = 1;
    this->text.little_endian = false;
}

CMxStream::~CMxStream() {
    free(psz_filepath);
    free(psz_name);
    free(psz_url);
    
    if (this->text.conv != (MxIconv)(-1))
        mxIconvClose(this->text.conv);
    
    if (this->peek != NULL)
        delete this->peek;
    if (this->block != NULL)
        delete this->block;
}

ssize_t CMxStream::vlc_stream_Read(void *buf, size_t len) {
    size_t copied = 0;
    
    while (len > 0)
    {
        ssize_t ret = vlc_stream_ReadPartial(buf, len);
        if (ret < 0)
            continue;
        if (ret == 0)
            break;
        
        if (buf != NULL)
            buf = (char *)buf + ret;
        assert(len >= (size_t)ret);
        len -= ret;
        copied += ret;
    }
    
    return copied;
}

ssize_t CMxStream::copyBlock(CMxBlock** ppBlock, void *buf, size_t len) {
    CMxBlock *pBlock = *ppBlock;
    
    if (block == NULL)
        return -1;
    
    if (len > pBlock->i_buffer)
        len = pBlock->i_buffer;
    
    if (buf != NULL)
        memcpy(buf, pBlock->p_buffer, len);
    
    pBlock->p_buffer += len;
    pBlock->i_buffer -= len;
    
    if (pBlock->i_buffer == 0)
    {
        delete *ppBlock;
        *ppBlock = NULL;
    }
    
    return likely(len > 0) ? (ssize_t)len : -1;
}

ssize_t CMxStream::readRaw(void *buf, size_t len) {
    ssize_t ret;
    assert(len <= SSIZE_MAX);
    
    /*if (vlc_killed())
        return 0;*/
    
    assert(this->block == NULL);
    if (buf == NULL)
    {
        if (unlikely(len == 0))
            return 0;
        
        //char dummy[(len <= 256 ? len : 256)];
		char *dummy = new char[len <= 256 ? len : 256];
        ret = read(dummy, sizeof (dummy));
		delete[] dummy;
    }
    else
        ret = read(buf, len);
    return ret;
    
    ret = copyBlock(&this->block, buf, len);
    if (ret >= 0)
        return ret;
    
    bool eof = false;
    readBlock(&eof);
    ret = copyBlock(&this->block, buf, len);
    if (ret >= 0)
        return ret;
    return eof ? 0 : -1;
}

ssize_t CMxStream::vlc_stream_ReadPartial(void *buf, size_t len) {

    ssize_t ret;
    ret = copyBlock(&this->peek, buf, len);
    if (ret >= 0)
    {
        offset += ret;
        assert(ret <= (ssize_t)len);
        return ret;
    }
    
    ret = readRaw(buf, len);
    if (ret > 0)
        offset += ret;
    if (ret == 0)
        eof = len != 0;
    assert(ret <= (ssize_t)len);
    return ret;
}

ssize_t CMxStream::vlc_stream_Peek(const uint8_t ** bufp, size_t len) {

    CMxBlock *peektmp;
    peektmp = peek;
    if (peektmp == NULL)
    {
        peektmp = block;
        this->peek = peektmp;
        this->block = NULL;
    }
    
    if (peektmp == NULL)
    {
        peektmp = CMxBlock::alloc(len);
        if (unlikely(peektmp == NULL))
            return MX_ENOMEM;
        
        peektmp->i_buffer = 0;
    }
    else if (peektmp->i_buffer < len)
        {
            size_t avail = peektmp->i_buffer;
            
            peektmp = peektmp->tryRealloc(0, len);
            if (unlikely(peektmp == NULL))
                return MX_ENOMEM;
            
            peektmp->i_buffer = avail;
        }
    
    this->peek = peektmp;
    *bufp = peektmp->p_buffer;
    
    while (peektmp->i_buffer < len)
    {
        size_t avail = peektmp->i_buffer;
        ssize_t ret;
        
        ret = readRaw(peektmp->p_buffer + avail, len - avail);
        if (ret < 0)
            continue;
        
        peektmp->i_buffer += ret;
        
        if (ret == 0)
            return peektmp->i_buffer;
    }
    
    return len;
}

CMxBlock* CMxStream::readBlock(bool *eof) {
    
    CMxBlock *pblock = CMxBlock::alloc(4096);
    if (unlikely(pblock == NULL))
        return NULL;
    
    ssize_t ret = read(pblock->p_buffer, pblock->i_buffer);
    if (ret > 0)
        pblock->i_buffer = ret;
    else
    {
        delete pblock;
        pblock = NULL;
    }
    
    return pblock;
}


CMxBlock* CMxStream::vlc_stream_ReadBlock() {
    CMxBlock *pblock;
    /*if (vlc_killed())
    {
        this->eof = true;
        return NULL;
    }*/
    
    if (this->peek != NULL)
    {
        pblock = this->peek;
        this->peek = NULL;
    }
    else if (this->block != NULL)
    {
        pblock = this->block;
        this->block = NULL;
    }
    /*else if (s->pf_block != NULL)
    {
        this->eof = false;
        block = s->pf_block(s, &this->eof);
    }
    else
    {
        block = block_Alloc(4096);
        if (unlikely(block == NULL))
            return NULL;
        
        ssize_t ret = read(s, block->p_buffer, block->i_buffer);
        if (ret > 0)
            block->i_buffer = ret;
        else
        {
            block_Release(block);
            block = NULL;
        }
        
        this->eof = !ret;
    }*/
    
    pblock = readBlock(&this->eof);
    
    this->eof = pblock == NULL;
    
    if (pblock != NULL)
        this->offset += block->i_buffer;
    
    return pblock;
}

uint64_t CMxStream::vlc_stream_Tell() {
    return this->offset;
}

bool CMxStream::vlc_stream_Eof() {
    return this->eof;
}

int CMxStream::vlc_stream_Seek(uint64_t offset) {
    
    this->eof = false;
    
    CMxBlock *peektmp = this->peek;
    if (peektmp != NULL)
    {
        if (offset >= this->offset
            && offset <= (this->offset + peektmp->i_buffer))
        {   /* Seeking within the peek buffer */
            size_t fwd = offset - this->offset;
            
            peektmp->p_buffer += fwd;
            peektmp->i_buffer -= fwd;
            this->offset = offset;
            
            if (peektmp->i_buffer == 0)
            {
                this->peek = NULL;
                delete peektmp;
            }
            
            return MX_SUCCESS;
        }
    }
    else
    {
        if (this->offset == offset)
            return MX_SUCCESS; /* Nothing to do! */
    }
    
    /*if (s->pf_seek == NULL)
        return MX_EGENERIC;
    
    int ret = s->pf_seek(s, offset);*/
    int ret = seek(offset);
    if (ret != MX_SUCCESS)
        return ret;
    
    this->offset = offset;
    
    if (peektmp != NULL)
    {
        this->peek = NULL;
        delete peektmp;
    }
    
    if (this->block != NULL)
    {
        delete this->block;
        this->block = NULL;
    }
    
    return MX_SUCCESS;
}

int CMxStream::vlc_stream_vaControl(int cmd, va_list args) {
    switch (cmd)
    {
        case STREAM_SET_TITLE:
        case STREAM_SET_SEEKPOINT:
        {
            int ret = control(cmd, args);
            if (ret != MX_SUCCESS)
                return ret;
            
            this->offset = 0;
            
            if (this->peek != NULL)
            {
                delete this->peek;
                this->peek = NULL;
            }
            
            if (this->block != NULL)
            {
                delete this->block;
                this->block = NULL;
            }
            
            return MX_SUCCESS;
        }
    }
    return control(cmd, args);
}
CMxBlock* CMxStream::vlc_stream_Block(size_t size) {
    if( unlikely(size > SSIZE_MAX) )
        return NULL;
    
    CMxBlock *pBlock = CMxBlock::alloc( size );
    if( unlikely(pBlock == NULL) )
        return NULL;
    
    ssize_t val = vlc_stream_Read(pBlock->p_buffer, size );
    if( val <= 0 )
    {
        delete block ;
        return NULL;
    }
    
    pBlock->i_buffer = val;
    return pBlock;
}

#define STREAM_PROBE_LINE 2048
#define STREAM_LINE_MAX (2048*100)
char* CMxStream::vlc_stream_ReadLine() {
    char *p_line = NULL;
    int i_line = 0, i_read = 0;
    
    /* Let's fail quickly if this is a readdir access */
    /*if( s->pf_read == NULL && s->pf_block == NULL )
        return NULL;*/
    
    for( ;; )
    {
        char *psz_eol;
        const uint8_t *p_data;
        int i_data;
        int64_t i_pos;
        
        /* Probe new data */
        i_data = vlc_stream_Peek(&p_data, STREAM_PROBE_LINE );
        if( i_data <= 0 ) break; /* No more data */
        
        /* BOM detection */
        i_pos = vlc_stream_Tell();
        if( i_pos == 0 && i_data >= 2 )
        {
            const char *psz_encoding = NULL;
            
            if( unlikely(this->text.conv != (MxIconv)-1) )
            {   /* seek back to beginning? reset */
                mxIconvClose( this->text.conv );
                this->text.conv = (MxIconv)-1;
            }
            
            if( !memcmp( p_data, "\xFF\xFE", 2 ) )
            {
                psz_encoding = "UTF-16LE";
                this->text.little_endian = true;
            }
            else if( !memcmp( p_data, "\xFE\xFF", 2 ) )
            {
                psz_encoding = "UTF-16BE";
            }
            
            /* Open the converter if we need it */
            if( psz_encoding != NULL )
            {
                MX_msgDbg((CMxObject*)NULL, "UTF-16 BOM detected" );
                this->text.conv = mxIconvOpen( "UTF-8", psz_encoding );
                if( unlikely(this->text.conv == (MxIconv)-1) )
                {
                    MX_msgErr( (CMxObject*)NULL, "iconv_open failed" );
                    goto error;
                }
                this->text.char_width = 2;
            }
        }
        
        if( i_data % this->text.char_width )
        {
            /* keep i_char_width boundary */
            i_data = i_data - ( i_data % this->text.char_width );
            MX_msgWarn( (CMxObject*)NULL, "the read is not i_char_width compatible");
        }
        
        if( i_data == 0 )
            break;
        
        /* Check if there is an EOL */
        if( this->text.char_width == 1 )
        {
            /* UTF-8: 0A <LF> */
            psz_eol = (char *)memchr( p_data, '\n', i_data );
            if( psz_eol == NULL )
            /* UTF-8: 0D <CR> */
                psz_eol = (char *)memchr( p_data, '\r', i_data );
        }
        else
        {
            const uint8_t *p_last = p_data + i_data - this->text.char_width;
            uint16_t eol = this->text.little_endian ? 0x0A00 : 0x00A0;
            
            assert( this->text.char_width == 2 );
            psz_eol = NULL;
            /* UTF-16: 000A <LF> */
            for( const uint8_t *p = p_data; p <= p_last; p += 2 )
            {
                if( U16_AT( p ) == eol )
                {
                    psz_eol = (char *)p + 1;
                    break;
                }
            }
            
            if( psz_eol == NULL )
            {   /* UTF-16: 000D <CR> */
                eol = this->text.little_endian ? 0x0D00 : 0x00D0;
                for( const uint8_t *p = p_data; p <= p_last; p += 2 )
                {
                    if( U16_AT( p ) == eol )
                    {
                        psz_eol = (char *)p + 1;
                        break;
                    }
                }
            }
        }
        
        if( psz_eol )
        {
            i_data = (psz_eol - (char *)p_data) + 1;
            p_line = (char*)realloc_or_free( p_line, i_line + i_data + this->text.char_width ); /* add \0 */
            if( !p_line )
                goto error;
            i_data = vlc_stream_Read(&p_line[i_line], i_data );
            if( i_data <= 0 ) break; /* Hmmm */
            i_line += i_data - this->text.char_width; /* skip \n */;
            i_read += i_data;
            
            /* We have our line */
            break;
        }
        
        /* Read data (+1 for easy \0 append) */
        p_line = (char*)realloc_or_free( p_line, i_line + STREAM_PROBE_LINE + this->text.char_width );
        if( !p_line )
            goto error;
        i_data = vlc_stream_Read(&p_line[i_line], STREAM_PROBE_LINE );
        if( i_data <= 0 ) break; /* Hmmm */
        i_line += i_data;
        i_read += i_data;
        
        if( i_read >= STREAM_LINE_MAX )
            goto error; /* line too long */
    }
    
    if( i_read > 0 )
    {
        if( this->text.char_width > 1 )
        {
            int i_new_line = 0;
            size_t i_in = 0, i_out = 0;
            const char * p_in = NULL;
            char * p_out = NULL;
            char * psz_new_line = NULL;
            
            /* iconv */
            /* UTF-8 needs at most 150% of the buffer as many as UTF-16 */
            i_new_line = i_line * 3 / 2 + 1;
            psz_new_line = (char*)malloc( i_new_line );
            if( psz_new_line == NULL )
                goto error;
            i_in = (size_t)i_line;
            i_out = (size_t)i_new_line;
            p_in = p_line;
            p_out = psz_new_line;
            
            if( mxIconv( this->text.conv, &p_in, &i_in, &p_out, &i_out ) == (size_t)-1 )
            {
                MX_msgErr( (CMxObject*)NULL, "conversion error: %s", mxStrerror_c( errno ) );
                MX_msgDbg( (CMxObject*)NULL, "original: %d, in %zu, out %zu", i_line, i_in, i_out );
            }
            free( p_line );
            p_line = psz_new_line;
            i_line = (size_t)i_new_line - i_out; /* does not include \0 */
        }
        
        /* Remove trailing LF/CR */
        while( i_line >= 1 &&
              (p_line[i_line - 1] == '\r' || p_line[i_line - 1] == '\n') )
            i_line--;
        
        /* Make sure the \0 is there */
        p_line[i_line] = '\0';
        
        return p_line;
    }
    
error:
    /* We failed to read any data, probably EOF */
    free( p_line );
    
    /* */
    if( this->text.conv != (MxIconv)(-1) )
    {
        mxIconvClose( this->text.conv );
        this->text.conv = (MxIconv)(-1);
    }
    return NULL;
}

int CMxStream::vlc_stream_ReadDir(input_item_node_t *p_node) {
    return readdir(p_node );
}

//void CMxStream::vlc_stream_Delete() {
//    stream_priv_t *priv = (stream_priv_t *)s;
//
//    priv->destroy(s);
//    stream_CommonDelete(s);
//}
//
//CMxStream* CMxStream::vlc_stream_CommonNew(vlc_object_t *, void (*)(CMxStream *)) {
//    stream_priv_t *priv = vlc_custom_create(parent, sizeof (*priv), "stream");
//    if (unlikely(priv == NULL))
//        return NULL;
//
//    stream_t *s = &priv->stream;
//
//    s->p_module = NULL;
//    s->psz_url = NULL;
//    s->p_source = NULL;
//    s->pf_read = NULL;
//    s->pf_block = NULL;
//    s->pf_readdir = NULL;
//    s->pf_seek = NULL;
//    s->pf_control = NULL;
//    s->p_sys = NULL;
//    s->p_input = NULL;
//    assert(destroy != NULL);
//    priv->destroy = destroy;
//    priv->block = NULL;
//    priv->peek = NULL;
//    priv->offset = 0;
//    priv->eof = false;
//
//    /* UTF16 and UTF32 text file conversion */
//    priv->text.conv = (vlc_iconv_t)(-1);
//    priv->text.char_width = 1;
//    priv->text.little_endian = false;
//
//    return s;
//}
CMxStream* CMxStream::vlc_stream_MemoryNew(CMxObject *obj, uint8_t *base,size_t size, bool preserve) {
    return NULL;
}
CMxStream* CMxStream::vlc_stream_NewURL(CMxObject *obj, const char *url) {
    return NULL;
}
CMxStream* CMxStream::vlc_stream_fifo_New(CMxObject *parent) {
    return NULL;
}
int CMxStream::vlc_stream_fifo_Queue(CMxStream *s, CMxBlock *block) {
    return 0;
}
ssize_t CMxStream::vlc_stream_fifo_Write(const void *buf,size_t len) {
    return 0;
}
void CMxStream::vlc_stream_fifo_Close() {
    
}
CMxStream* CMxStream::vlc_stream_FilterNew( CMxStream *p_source, const char *psz_stream_filter ) {
    return NULL;
}
int CMxStream::vlc_stream_FilterDefaultReadDir(input_item_node_t *p_node) {
    return 0;
}
