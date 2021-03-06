/*****************************************************************************
 * vlc_boxes.h : Boxes/Atoms handling helpers
 *****************************************************************************/
#ifndef CMXBOXES_H
#define CMXBOXES_H

#include "MxCommon.h"
#include "../MxSystem/CMxBlock.h"

/**
 * \file
 * This file defines functions, structures for handling boxes/atoms in vlc
 */

class CMxBox {
public:
	CMxBox(int i_size) {
		b = CMxBlock::alloc(i_size);
	}
	~CMxBox() {
		if (b) {
			b->release();
			b = nullptr;
		}
	}

	void release() {
		if (b)
			b->release();
		delete this;
	}

	int bo_extend(size_t i_total)
	{
		const size_t i_size = b->i_size - (b->p_buffer - b->p_start);
		if (i_total >= i_size)
		{
			int i_growth = basesize;
			while (i_total >= i_size + i_growth)
				i_growth += p_bo->basesize;

			int i = p_bo->b->i_buffer; /* Realloc would set payload size == buffer size */
			p_bo->b = block_Realloc(p_bo->b, 0, i_size + i_growth);
			if (!p_bo->b)
				return false;
			p_bo->b->i_buffer = i;
		}
		return true;
	}

private:
	CMxBlock* b = nullptr;
};

typedef struct bo_t
{
    block_t     *b;
    size_t      basesize;
} bo_t;

static inline bool bo_init(bo_t *p_bo, int i_size)
{
    p_bo->b = block_Alloc(i_size);
    if (p_bo->b == NULL)
        return false;

    p_bo->b->i_buffer = 0;
    p_bo->basesize = i_size;

    return true;
}

static inline void bo_deinit(bo_t *p_bo)
{
    if(p_bo->b)
        block_Release(p_bo->b);
}

static inline void bo_free(bo_t *p_bo)
{
    if(!p_bo)
        return;
    bo_deinit(p_bo);
    free(p_bo);
}

static inline int bo_extend(bo_t *p_bo, size_t i_total)
{
    if(!p_bo->b)
        return false;
    const size_t i_size = p_bo->b->i_size - (p_bo->b->p_buffer - p_bo->b->p_start);
    if (i_total >= i_size)
    {
        int i_growth = p_bo->basesize;
        while(i_total >= i_size + i_growth)
            i_growth += p_bo->basesize;

        int i = p_bo->b->i_buffer; /* Realloc would set payload size == buffer size */
        p_bo->b = block_Realloc(p_bo->b, 0, i_size + i_growth);
        if (!p_bo->b)
            return false;
        p_bo->b->i_buffer = i;
    }
    return true;
}

#define BO_SET_DECL_S(func, handler, type) static inline bool func(bo_t *p_bo, size_t i_offset, type val)\
    {\
        if (!bo_extend(p_bo, i_offset + sizeof(type)))\
            return false;\
        handler(&p_bo->b->p_buffer[i_offset], val);\
        return true;\
    }

#define BO_ADD_DECL_S(func, handler, type) static inline bool func(bo_t *p_bo, type val)\
    {\
        if(!p_bo->b || !handler(p_bo, p_bo->b->i_buffer, val))\
            return false;\
        p_bo->b->i_buffer += sizeof(type);\
        return true;\
    }

#define BO_FUNC_DECL(suffix, handler, type ) \
    BO_SET_DECL_S( bo_set_ ## suffix ## be, handler ## BE, type )\
    BO_SET_DECL_S( bo_set_ ## suffix ## le, handler ## LE, type )\
    BO_ADD_DECL_S( bo_add_ ## suffix ## be, bo_set_ ## suffix ## be, type )\
    BO_ADD_DECL_S( bo_add_ ## suffix ## le, bo_set_ ## suffix ## le, type )

static inline bool bo_set_8(bo_t *p_bo, size_t i_offset, uint8_t i)
{
    if (!bo_extend(p_bo, i_offset + 1))
        return false;
    p_bo->b->p_buffer[i_offset] = i;
    return true;
}

static inline bool bo_add_8(bo_t *p_bo, uint8_t i)
{
    if(!p_bo->b || !bo_set_8( p_bo, p_bo->b->i_buffer, i ))
        return false;
    p_bo->b->i_buffer++;
    return true;
}

/* declares all bo_[set,add]_[16,32,64] */
BO_FUNC_DECL( 16, SetW,  uint16_t )
BO_FUNC_DECL( 32, SetDW, uint32_t )
BO_FUNC_DECL( 64, SetQW, uint64_t )

#undef BO_FUNC_DECL
#undef BO_SET_DECL_S
#undef BO_ADD_DECL_S

static inline bool bo_add_24be(bo_t *p_bo, uint32_t i)
{
    if(!p_bo->b || !bo_extend(p_bo, p_bo->b->i_buffer + 3))
        return false;
    p_bo->b->p_buffer[p_bo->b->i_buffer++] = ((i >> 16) & 0xff);
    p_bo->b->p_buffer[p_bo->b->i_buffer++] = ((i >> 8) & 0xff);
    p_bo->b->p_buffer[p_bo->b->i_buffer++] = (i & 0xff);
    return true;
}

static inline void bo_swap_32be (bo_t *p_bo, size_t i_pos, uint32_t i)
{
    if (!p_bo->b || p_bo->b->i_buffer < i_pos + 4)
        return;
    p_bo->b->p_buffer[i_pos    ] = (i >> 24)&0xff;
    p_bo->b->p_buffer[i_pos + 1] = (i >> 16)&0xff;
    p_bo->b->p_buffer[i_pos + 2] = (i >>  8)&0xff;
    p_bo->b->p_buffer[i_pos + 3] = (i      )&0xff;
}

static inline bool bo_add_mem(bo_t *p_bo, size_t i_size, const void *p_mem)
{
    if(!p_bo->b || !bo_extend(p_bo, p_bo->b->i_buffer + i_size))
        return false;
    memcpy(&p_bo->b->p_buffer[p_bo->b->i_buffer], p_mem, i_size);
    p_bo->b->i_buffer += i_size;
    return true;
}

#define bo_add_fourcc(p_bo, fcc) bo_add_mem(p_bo, 4, fcc)

#endif // CMXBOXES_H
