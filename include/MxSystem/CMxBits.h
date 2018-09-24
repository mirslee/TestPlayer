#ifndef MXBITS_H
#define MXBITS_H 1

#include "MxTypes.h"
#include "MxCommon.h"

/**
 * \file
 * This file defines class for handling streams of bits
 */

class MXSYSTEM_API CMxBits {

public:
	CMxBits(uint8_t *p_data, size_t i_data, bool readOnly);
	int pos();
	int remain();
	int eof();
    uint32_t read(int i_count );
	void forward(int i_count);
	uint32_t read1();
	uint32_t show(int i_count );
	void skip(ssize_t i_count );
	void write(int i_count, uint32_t i_bits );
	bool aligned();
	void align();
	void align0();
	void align1();
	/* Read unsigned Exp-Golomb code */
	uint_fast32_t bs_read_ue();
	/* Read signed Exp-Golomb code */
	int_fast32_t bs_read_se();

private:
	uint8_t *p_start;
	uint8_t *p;
	uint8_t *p_end;

	ssize_t  i_left;    /* i_count number of available bits */
	bool     b_read_only;

	/* forward read modifier (p_start, p_end, p_fwpriv, count) */
	//uint8_t *(*pf_forward)(uint8_t *, uint8_t *, void *, size_t);
	void    *p_fwpriv;
};

#endif
