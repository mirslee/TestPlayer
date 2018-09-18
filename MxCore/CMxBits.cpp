#include "stdafx.h"
#include "CMxBits.h"

CMxBits::CMxBits(uint8_t *p_data, size_t i_data, bool readOnly) {
	p_start = p_data;
	p       = p_start;
	p_end   = p_start + i_data;
	i_left  = 8;
	b_read_only = readOnly;
	p_fwpriv = NULL;
	//s->pf_forward = NULL;
}

int CMxBits::pos() {
	return 8 * ( p - p_start ) + 8 - i_left;
}

int CMxBits::remain()
{
	if( p >= p_end )
		return 0;
	else
		return 8 * ( p_end - p ) - 8 + i_left;
}

int CMxBits::eof()
{
	return( p >= p_end ? 1: 0 );
}

void CMxBits::forward(int i_count) {
	p = p + i_count;
	//#define bs_forward( s, i ) \
	//	s->p = s->pf_forward ? s->pf_forward( s->p, s->p_end, s->p_fwpriv, i ) : s->p + i
}

uint32_t CMxBits::read(int i_count )
{
	static const uint32_t i_mask[33] =
	{  0x00,
	0x01,      0x03,      0x07,      0x0f,
	0x1f,      0x3f,      0x7f,      0xff,
	0x1ff,     0x3ff,     0x7ff,     0xfff,
	0x1fff,    0x3fff,    0x7fff,    0xffff,
	0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
	0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
	0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
	0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};;

	int      i_shr, i_drop = 0;
	uint32_t i_result = 0;

	if( i_count > 32 )
	{
		i_drop = i_count - 32;
		i_count = 32;
	}

	while( i_count > 0 )
	{
		if( p >= p_end )
		{
			break;
		}

		if( ( i_shr = i_left - i_count ) >= 0 )
		{
			/* more in the buffer than requested */
			i_result |= ( *p >> i_shr )&i_mask[i_count];
			i_left -= i_count;
			if( i_left == 0 )
			{
				forward(1);
				i_left = 8;
			}
			break;
		}
		else
		{
			/* less in the buffer than requested */
			if( -i_shr == 32 )
				i_result = 0;
			else
				i_result |= (*p&i_mask[i_left]) << -i_shr;
			i_count  -= i_left;
			forward(1);
			i_left = 8;
		}
	}

	if( i_drop )
		forward(i_drop);

	return i_result;
}

uint32_t CMxBits::read1()
{
	if( p < p_end )
	{
		unsigned int i_result;

		i_left--;
		i_result = ( *p >> i_left )&0x01;
		if( i_left == 0 )
		{
			forward(1);
			i_left = 8;
		}
		return i_result;
	}

	return 0;
}

uint32_t CMxBits::show(int i_count )
{
	CMxBits s_tmp = *this;
	return s_tmp.read(i_count );
}

void CMxBits::skip(ssize_t i_count )
{
	i_left -= i_count;

	if( i_left <= 0 )
	{
		const size_t i_bytes = 1 + i_left / -8;
		forward(i_bytes);
		if( i_bytes * 8 < i_bytes /* ofw */ )
			i_left = i_bytes;
		else
			i_left += 8 * i_bytes;
	}
}

void CMxBits::write(int i_count, uint32_t i_bits )
{
	if(b_read_only )
		return;

	while( i_count > 0 )
	{
		if( p >= p_end )
		{
			break;
		}

		i_count--;

		if( ( i_bits >> i_count )&0x01 )
		{
			*p |= 1 << ( i_left - 1 );
		}
		else
		{
			*p &= ~( 1 << ( i_left - 1 ) );
		}
		i_left--;
		if( i_left == 0 )
		{
			forward(1);
			i_left = 8;
		}
	}
}

bool CMxBits::aligned()
{
	return i_left % 8 == 0;
}

void CMxBits::align()
{
	if( i_left != 8 )
	{
		i_left = 8;
		p++;
	}
}

void CMxBits::align0()
{
	if( i_left != 8 )
	{
		write(i_left, 0 );
	}
}

void CMxBits::align1()
{
	while( !b_read_only && i_left != 8 )
	{
		write(1, 1 );
	}
}

/* Read unsigned Exp-Golomb code */
uint_fast32_t CMxBits::bs_read_ue()
{
	unsigned i = 0;

	while( read1() == 0 && p < p_end && i < 31 )
		i++;

	return (1U << i) - 1 + read( i );
}

/* Read signed Exp-Golomb code */
int_fast32_t CMxBits::bs_read_se()
{
	uint_fast32_t val = bs_read_ue();

	return (val & 0x01) ? (int_fast32_t)((val + 1) / 2)
		: -(int_fast32_t)(val / 2);
}