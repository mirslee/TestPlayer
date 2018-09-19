#include "stdafx.h"
#include "CMxBlock.h"
#include "MxConfig.h"
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "MxCommon.h"
#include "MxFileSystem.h"

/** Initial memory alignment of data block.
 * @note This must be a multiple of sizeof(void*) and a power of two.
 * libavcodec AVX optimizations require at least 32-bytes. */
#define BLOCK_ALIGN        32

/** Initial reserved header and footer size. */
#define BLOCK_PADDING      32

CMxBlock* CMxBlock::alloc(size_t size) {
    if (unlikely(size >> 27))
    {
        errno = ENOBUFS;
        return NULL;
    }
    
    /* 2 * BLOCK_PADDING: pre + post padding */
    const size_t alloc = sizeof (CMxBlock) + BLOCK_ALIGN + (2 * BLOCK_PADDING)
    + size;
    if (unlikely(alloc <= size))
        return NULL;
    
    CMxBlock *b = (CMxBlock*)malloc (alloc);
    if (unlikely(b == NULL))
        return NULL;
    
    b->init(b + 1, alloc - sizeof (*b));
    static_assert ((BLOCK_PADDING % BLOCK_ALIGN) == 0,
                   "BLOCK_PADDING must be a multiple of BLOCK_ALIGN");
    b->p_buffer += BLOCK_PADDING + BLOCK_ALIGN - 1;
    b->p_buffer = (uint8_t *)(((uintptr_t)b->p_buffer) & ~(BLOCK_ALIGN - 1));
    b->i_buffer = size;
    return b;
}

void CMxBlock::init(void* buf, size_t size) {
    this->p_next = NULL;
    this->p_buffer = (uint8_t *)buf;
    this->i_buffer = size;
    this->p_start = (uint8_t *)buf;
    this->i_size = size;
    this->i_flags = 0;
    this->i_nb_samples = 0;
    this->i_pts = MX_TS_INVALID;
    this->i_dts = MX_TS_INVALID;
    this->i_length = 0;
}

void CMxBlock::check() {
    CMxBlock * block = this;
    while (block != NULL)
    {
        unsigned char *start = block->p_start;
        unsigned char *end = block->p_start + block->i_size;
        unsigned char *bufstart = block->p_buffer;
        unsigned char *bufend = block->p_buffer + block->i_buffer;
        
        assert (start <= end);
        assert (bufstart <= bufend);
        assert (bufstart >= start);
        assert (bufend <= end);
        
        block = block->p_next;
    }
}

void CMxBlock::release() {
    assert (this->p_start == (unsigned char *)(this + 1));
    this->p_next = NULL;
    check();
    delete this;
}
void CMxBlock::metaCopy(CMxBlock* from) {
    this->p_next    = from->p_next;
    this->i_nb_samples = from->i_nb_samples;
    this->i_dts     = from->i_dts;
    this->i_pts     = from->i_pts;
    this->i_flags   = from->i_flags;
    this->i_length  = from->i_length;
}
void CMxBlock::invalidate() {
    this->p_next = NULL;
    check();
}
void CMxBlock::heapRelease() {
    invalidate();
    delete[] p_start;
    delete this;
}
CMxBlock* CMxBlock::tryRealloc(ssize_t i_prebody, size_t i_body){
    check();
    
    /* Corner case: empty block requested */
    if( i_prebody <= 0 && i_body <= (size_t)(-i_prebody) )
        i_prebody = i_body = 0;
    
    assert( this->p_start <= this->p_buffer );
    assert( this->p_start + this->i_size
           >= this->p_buffer + this->i_buffer );
    
    /* First, shrink payload */
    
    /* Pull payload start */
    if( i_prebody < 0 )
    {
        if( this->i_buffer >= (size_t)-i_prebody )
        {
            this->p_buffer -= i_prebody;
            this->i_buffer += i_prebody;
        }
        else /* Discard current payload entirely */
            this->i_buffer = 0;
        i_body += i_prebody;
        i_prebody = 0;
    }
    
    /* Trim payload end */
    if( this->i_buffer > i_body )
        this->i_buffer = i_body;
    
    size_t requested = i_prebody + i_body;
    
    if( this->i_buffer == 0 )
    {   /* Corner case: nothing to preserve */
        if( requested <= this->i_size )
        {   /* Enough room: recycle buffer */
            size_t extra = this->i_size - requested;
            
            this->p_buffer = this->p_start + (extra / 2);
            this->i_buffer = requested;
            return this;
        }
        
        /* Not enough room: allocate a new buffer */
        CMxBlock *p_rea = this->alloc( requested );
        if( p_rea == NULL )
            return NULL;
        
        p_rea->metaCopy(this);
        this->release();
        return p_rea;
    }
    
    uint8_t *p_start = this->p_start;
    uint8_t *p_end = p_start + this->i_size;
    
    /* Second, reallocate the buffer if we lack space. */
    assert( i_prebody >= 0 );
    if( (size_t)(this->p_buffer - p_start) < (size_t)i_prebody
       || (size_t)(p_end - this->p_buffer) < i_body )
    {
        CMxBlock *p_rea = alloc( requested );
        if( p_rea == NULL )
            return NULL;
        
        memcpy( p_rea->p_buffer + i_prebody, this->p_buffer,
               this->i_buffer );
        p_rea->metaCopy(this);
        this->release();
        return p_rea;
    }
    
    /* Third, expand payload */
    
    /* Push payload start */
    if( i_prebody > 0 )
    {
        this->p_buffer -= i_prebody;
        this->i_buffer += i_prebody;
        i_body += i_prebody;
        i_prebody = 0;
    }
    
    /* Expand payload to requested size */
    this->i_buffer = i_body;
    
    return this;
}
CMxBlock* CMxBlock::realloc(CMxBlock *, ssize_t pre, size_t body){
    CMxBlock *rea = tryRealloc(pre, body);
    if (rea == NULL)
        release();
    return rea;
}
CMxBlock* CMxBlock::heapAlloc(void *addr, size_t length){
    CMxBlock *block = new CMxBlock();
    if (block == NULL)
    {
        free (addr);
        return NULL;
    }
    block->init(addr, length);
    return block;
}
CMxBlock* CMxBlock::mmapAlloc(void *addr, size_t length){
    return NULL;
}
CMxBlock* CMxBlock::shmAlloc(void *addr, size_t length){
    return NULL;
}
CMxBlock* CMxBlock::file(int fd, bool write){
    return NULL;
}
CMxBlock* CMxBlock::filePath(const char * path, bool write){
    int fd = mxOpen (path, O_RDONLY);
    if (fd == -1)
        return NULL;
    
    CMxBlock *block = file(fd, write);
    mxClose (fd);
    return block;
}
CMxBlock* CMxBlock::fifoNew(void){
    return NULL;
}
void CMxBlock::fifoRelease(MxBlockFifo *){
    
}
void CMxBlock::fifoEmpty(MxBlockFifo *){
    
}
void CMxBlock::fifoPut(MxBlockFifo *fifo, CMxBlock *block){
    
}
CMxBlock* CMxBlock::fifoGet(MxBlockFifo *){
    return NULL;
}
CMxBlock* CMxBlock::fifoShow(MxBlockFifo *){
    return NULL;
}
size_t CMxBlock::fifoCount(MxBlockFifo *){
    return 0;
}
void CMxBlock::fifoLock(MxBlockFifo *){
}
void CMxBlock::fifoUnlock(MxBlockFifo *){
    
}
void CMxBlock::fifoSignal(MxBlockFifo *){
    
}
void CMxBlock::fifoWait(MxBlockFifo *){
    
}
void CMxBlock::fifoWaitCond(MxBlockFifo *, MxCond *){
    
}
void CMxBlock::fifoQueueUnlocked(MxBlockFifo *, CMxBlock *){
    
}
CMxBlock* CMxBlock::fifoDequeueUnlocked(MxBlockFifo *){
    return NULL;
}
CMxBlock* CMxBlock::fifoDequeueAllUnlocked(MxBlockFifo *){
    return NULL;
}
size_t CMxBlock::fifoGetCount(const MxBlockFifo *){
    return 0;
}
size_t CMxBlock::fifoGetBytes(const MxBlockFifo *){
    return 0;
}




