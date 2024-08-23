#ifndef MMATH_H
#define MMATH_H

#include <stdint.h>

/*
 * Mihir Math
 *
 * Some whack math, don't worry about it
 */

/* Currently never planning on finishing */
int16_t mm_tan(int16_t x);
int16_t mm_cot(int16_t x);

uint8_t mm_convert_theta(uint16_t *theta);

/* Table lookups */
uint16_t mm_tant(uint16_t x);
uint16_t mm_cott(uint16_t x);
uint16_t mm_sect(uint16_t x);
uint16_t mm_csct(uint16_t x);
uint16_t mm_sint(uint16_t x);
uint16_t mm_cost(uint16_t x);

/* Macros for multiply by a 8:8 (integer:fraction) fixed point number */
#define MM_TAN_MUL(t, x) (((uint32_t) mm_tant(t) * (uint32_t) (x)) >>  8)
#define MM_COT_MUL(t, x) (((uint32_t) mm_cott(t) * (uint32_t) (x)) >>  8)
#define MM_SEC_MUL(t, x) (((uint32_t) mm_sect(t) * (uint32_t) (x)) >>  8)
#define MM_CSC_MUL(t, x) (((uint32_t) mm_csct(t) * (uint32_t) (x)) >>  8)
#define MM_SIN_MUL(t, x) (((uint32_t) mm_sint(t) * (uint32_t) (x)) >> 16)
#define MM_COS_MUL(t, x) (((uint32_t) mm_cost(t) * (uint32_t) (x)) >> 16)

uint16_t mm_atant(uint16_t x, uint16_t y);

#endif