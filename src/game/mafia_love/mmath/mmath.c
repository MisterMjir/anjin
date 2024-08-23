#include "mmath.h"

/*
 * References
 * https://www.nullhardware.com/blog/fixed-point-sine-and-cosine-for-embedded-systems/
 * http://www.coranac.com/2009/07/sines/
 */
 
 /*
  * Trig constants (2 pi isn't actually 2 pi because it is 0, but the constant is still there)
	*/
#define PI  0x8000
#define HPI 0x4000

/*
 * tan
 *
 * @desc
 *   Calculates a fixed point tangent output
 *   using a 9th degree maclaurian series
 *   of tangent
 *   The domain is [0, pi / 2], if x is out of the domain
 *   it will get converted into the domain by 90-degree rotations
 * @param
 *   16-bit angle
 *   2^16 - 1 is ~ 2 * pi
 * @return
 *   16-bit fixed point result
 *   Higher byte is integer part
 *   Lower byte is fractional part
 */
int16_t mm_tan(int16_t x)
{
	/*
	 * 1. Convert the input to [0, pi / 2]
	 */
	while (x > HPI) x -= HPI;
	return 0;
}

/*
 * cot
 *
 * @desc
 *   Because of the restrictions of tan,
 *   cot is just tan mirrored about x = pi / 2,
 *   so cot just converts the input and calls tan
 * @param x
 *   Same as tan
 * @return
 *   Same as tan
 */
int16_t mm_cot(int16_t x)
{
	/*
	 * 1. Convert the input to [0, pi / 2]
	 */
	while (x > HPI) x -= HPI;
	/*
	 * 2. Call tangent
	 */
	return mm_tan(HPI - x);
}

uint8_t mm_convert_theta(uint16_t *theta)
{
	uint8_t data;
	
	/* Convert theta */
  data = 0;
  while (*theta > 0x4000) {
		*theta -= 0x4000;
		++data;
  }
  if (data & 0x1) *theta = 16384 - *theta; /* Make sure theta is based off x-axis and not y */
  if (data > 1) data ^= 0x1; /* Convert quadrant to format specified above */
	
	return data;
}

/*
 * trig tables (mm_[trig]t)
 *
 * @desc
 *   Taylor stuff is hard to figure out, I'm gonna sacrifice
 *   ~ 32Kb instead for each table (the co-tables are just
 *   mirrors of the other tables in the domain of the functions)
 */
#include "trig_tables.h"
uint16_t mm_tant(uint16_t x)
{
	return mm_tan_table[x];
}

uint16_t mm_cott(uint16_t x)
{
	return mm_tan_table[HPI - x];
}

uint16_t mm_sect(uint16_t x)
{
	return mm_sec_table[x];
}

uint16_t mm_csct(uint16_t x)
{
	return mm_sec_table[HPI - x];
}

uint16_t mm_sint(uint16_t x)
{
	return mm_sin_table[x];
}

uint16_t mm_cost(uint16_t x)
{
	return mm_sin_table[HPI - x];
}

uint16_t mm_atant(uint16_t x, uint16_t y)
{
	// original code didn't need this safety check
	if (!x) x = 1;
	uint16_t r = (uint32_t) (y << 8) / (uint32_t) x;
	if (r >= 0x1400) return HPI;
	else return mm_atan_table[r];
}