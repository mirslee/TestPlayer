#ifndef CMXBLOCK_H
#define CMXBLOCK_H 1

/**
 * \defgroup block Data blocks
 * \ingroup input
 *
 * Blocks of binary data.
 *
 * @ref block_t is a generic structure to represent a binary blob within VLC.
 * The primary goal of the structure is to avoid memory copying as data is
 * passed around. It is notably used between the \ref demux, the packetizer
 * (if present) and the \ref decoder, and for audio, between the \ref decoder,
 * the audio filters, and the \ref audio_output.
 *
 * @{
 * \file
 * Data block definition and functions
 */

#include <sys/types.h>  /* for ssize_t */

/****************************************************************************
 * block:
 ****************************************************************************
 * - i_flags may not always be set (ie could be 0, even for a key frame
 *      it depends where you receive the buffer (before/after a packetizer
 *      and the demux/packetizer implementations.
 * - i_dts/i_pts could be VLC_TS_INVALID, it means no pts/dts
 * - i_length: length in microseond of the packet, can be null except in the
 *      sout where it is mandatory.
 *
 * - i_buffer number of valid data pointed by p_buffer
 *      you can freely decrease it but never increase it yourself
 *      (use block_Realloc)
 * - p_buffer: pointer over datas. You should never overwrite it, you can
 *   only incremment it to skip datas, in others cases use block_Realloc
 *   (don't duplicate yourself in a bigger buffer, block_Realloc is
 *   optimised for preheader/postdatas increase)
 ****************************************************************************/

/** The content doesn't follow the last block, possible some blocks in between
 *  have been lost */
#define BLOCK_FLAG_DISCONTINUITY 0x0001
/** Intra frame */
#define BLOCK_FLAG_TYPE_I        0x0002
/** Inter frame with backward reference only */
#define BLOCK_FLAG_TYPE_P        0x0004
/** Inter frame with backward and forward reference */
#define BLOCK_FLAG_TYPE_B        0x0008
/** For inter frame when you don't know the real type */
#define BLOCK_FLAG_TYPE_PB       0x0010
/** Warn that this block is a header one */
#define BLOCK_FLAG_HEADER        0x0020
/** This block contains the last part of a sequence  */
#define BLOCK_FLAG_END_OF_SEQUENCE 0x0040
/** This block contains a clock reference */
#define BLOCK_FLAG_CLOCK         0x0080
/** This block is scrambled */
#define BLOCK_FLAG_SCRAMBLED     0x0100
/** This block has to be decoded but not be displayed */
#define BLOCK_FLAG_PREROLL       0x0200
/** This block is corrupted and/or there is data loss  */
#define BLOCK_FLAG_CORRUPTED     0x0400
/** This block contains an interlaced picture with top field stored first */
#define BLOCK_FLAG_TOP_FIELD_FIRST 0x0800
/** This block contains an interlaced picture with bottom field stored first */
#define BLOCK_FLAG_BOTTOM_FIELD_FIRST 0x1000
/** This block contains a single field from interlaced picture. */
#define BLOCK_FLAG_SINGLE_FIELD  0x2000

/** This block contains an interlaced picture */
#define BLOCK_FLAG_INTERLACED_MASK \
(BLOCK_FLAG_TOP_FIELD_FIRST|BLOCK_FLAG_BOTTOM_FIELD_FIRST|BLOCK_FLAG_SINGLE_FIELD)

#define BLOCK_FLAG_TYPE_MASK \
(BLOCK_FLAG_TYPE_I|BLOCK_FLAG_TYPE_P|BLOCK_FLAG_TYPE_B|BLOCK_FLAG_TYPE_PB)

/* These are for input core private usage only */
#define BLOCK_FLAG_CORE_PRIVATE_MASK  0x00ff0000
#define BLOCK_FLAG_CORE_PRIVATE_SHIFT 16

/* These are for module private usage only */
#define BLOCK_FLAG_PRIVATE_MASK  0xff000000
#define BLOCK_FLAG_PRIVATE_SHIFT 24

#include "MxThread.h"

class CMxBlock;
struct MxBlockFifo
{
    MxMutex         lock;
    MxCond          wait;
    CMxBlock        *p_first;
    CMxBlock        **pp_last;
    size_t          i_depth;
    size_t          i_size;
};

class MXCODEC_API CMxBlock {
public:
    static CMxBlock* alloc(size_t size);
    static CMxBlock *heapAlloc(void *, size_t);
    void init(void* buf, size_t size);
    void metaCopy(CMxBlock* from);
    CMxBlock *tryRealloc(ssize_t pre, size_t body);
    CMxBlock *realloc(CMxBlock *, ssize_t pre, size_t body);
    CMxBlock *mmapAlloc(void *addr, size_t length);
    CMxBlock * shmAlloc(void *addr, size_t length);
    CMxBlock *file(int fd, bool write);
    CMxBlock *filePath(const char *, bool write);
    CMxBlock *fifoNew(void);
    void fifoRelease(MxBlockFifo *);
    void fifoEmpty(MxBlockFifo *);
    void fifoPut(MxBlockFifo *fifo, CMxBlock *block);
    CMxBlock *fifoGet(MxBlockFifo *);
    CMxBlock *fifoShow(MxBlockFifo *);
    size_t fifoCount(MxBlockFifo *);
    void fifoLock(MxBlockFifo *);
    void fifoUnlock(MxBlockFifo *);
    void fifoSignal(MxBlockFifo *);
    void fifoWait(MxBlockFifo *);
    void fifoWaitCond(MxBlockFifo *, MxCond *);
    void fifoQueueUnlocked(MxBlockFifo *, CMxBlock *);
    CMxBlock *fifoDequeueUnlocked(MxBlockFifo *);
    CMxBlock *fifoDequeueAllUnlocked(MxBlockFifo *);
    size_t fifoGetCount(const MxBlockFifo *);
    size_t fifoGetBytes(const MxBlockFifo *);
    
    void release();
    
private:
    CMxBlock();
    void check();
    void invalidate();
    void heapRelease();
    
public:
    CMxBlock* p_next;
    uint8_t    *p_buffer; /**< Payload start */
    size_t      i_buffer; /**< Payload length */
    uint8_t    *p_start; /**< Buffer start */
    size_t      i_size; /**< Buffer total size */
    
    uint32_t    i_flags;
    unsigned    i_nb_samples; /* Used for audio */
    
    mtime_t     i_pts;
    mtime_t     i_dts;
    mtime_t     i_length;
    
    /* Rudimentary support for overloading block (de)allocation. */
    //block_free_t pf_release;
};

#endif
