#include "level_inputs.h"
#include "../mmath/mmath.h"

/* Joysticks just work as on off basically */

#define INPUT_LOW 1024
#define INPUT_HIGH 3072

#define INPUT_LOW2 512
#define INPUT_HIGH2 3584
/* Off is in between high and low */

/*
 * get_thetai
 *
 * @desc
 *   Get theta of inputs, this theta
 *   Is what angle to move
 * @param input
 *   An input struct
 * @param phi
 *   Player angle (Unused)
 * @return
 *   Theta to move in
 */
#define PI   32768
#define PI_2 16384
#define PI_4  8192
static uint16_t get_thetai(struct input *inputs, uint16_t phi)
{
  uint16_t thetai;
	
	if (inputs->y1 > INPUT_HIGH2) {
		if 			(inputs->x1 < INPUT_LOW) 	{ thetai = 0 + PI_4; }
		else if (inputs->x1 > INPUT_HIGH) { thetai = 0 - PI_4; }
		else 															{ thetai = 0; }
	}
	else if (inputs->y1 < INPUT_LOW) {
		if 			(inputs->x1 < INPUT_LOW)  { thetai = PI - PI_4; }
		else if (inputs->x1 > INPUT_HIGH) { thetai = PI + PI_4; }
		else 															{ thetai = PI; }
	}
	else if (inputs->x1 < INPUT_LOW) {
		thetai = 0 + PI_2;
	}
	else if (inputs->x1 > INPUT_HIGH) {
		thetai = 0 - PI_2;
	}

  return thetai;
}

static void move_quad(uint16_t *x, uint16_t *y, uint16_t dx, uint16_t dy, uint8_t quad)
{
  if      (quad == 0) { *y -= dy; *x += dx; } /* Q1 */
  else if (quad == 1) { *y -= dy; *x -= dx; } /* Q2 */
  else if (quad == 2) { *y += dy; *x += dx; } /* Q4 */
  else if (quad == 3) { *y += dy; *x -= dx; } /* Q3 */
}

#define CR 0x0010
#define MR 0x0020
void move(struct input *input, uint16_t* x, uint16_t* y, uint16_t theta, uint8_t quad, const uint8_t* world, uint8_t worldw)
{
  uint16_t cdx, cdy, mdx, mdy; /* Collision and move deltas */
	uint32_t mrx, mry;
	
	/*
	if (input->x1 < 1024)
		mrx = ((uint32_t) (1024 - input->x1) * (uint32_t) MR) >> 10;
	if (input->x1 > 2048)
		mrx = ((uint32_t) (input->x1 - 2048) * (uint32_t) MR) >> 10;
	if (input->y1 < 1024)
		mrx = ((uint32_t) (1024 - input->y1) * (uint32_t) MR) >> 10;
	if (input->y1 > 2048)
		mrx = ((uint32_t) (input->y1 - 2048) * (uint32_t) MR) >> 10;
	*/

  cdx = ((uint32_t) mm_cost(theta) * (uint32_t) CR) >> 16;
  cdy = ((uint32_t) mm_sint(theta) * (uint32_t) CR) >> 16;
  mdx = ((uint32_t) mm_cost(theta) * (uint32_t) MR) >> 16;
  mdy = ((uint32_t) mm_sint(theta) * (uint32_t) MR) >> 16;

  move_quad(x, y, mdx, mdy, quad);

  /* Check 8 points of the circle */
  if ((world[((*y + cdy) >> 8) * worldw + ((*x + cdx) >> 8)]) ||
      (world[((*y + cdy) >> 8) * worldw + ((*x - cdx) >> 8)]) ||
      (world[((*y - cdy) >> 8) * worldw + ((*x + cdx) >> 8)]) ||
      (world[((*y - cdy) >> 8) * worldw + ((*x - cdx) >> 8)]) ||
      (world[((*y - CR ) >> 8) * worldw + ((*x      ) >> 8)]) ||
      (world[((*y      ) >> 8) * worldw + ((*x + CR ) >> 8)]) ||
      (world[((*y + CR ) >> 8) * worldw + ((*x      ) >> 8)]) ||
      (world[((*y      ) >> 8) * worldw + ((*x - CR ) >> 8)])
    ) {
    /* Handle collision (just move backwards to negate the move)*/
    mdy = 0 - mdy;
    mdx = 0 - mdx;

    move_quad(x, y, mdx, mdy, quad);
  }
}
#undef MR
#undef CR

/*
 * turn
 *
 * @desc
 *   Turns/rotates the player (change player phi)
 * @param input
 *   An input struct
 * @param phi
 *   Player phi
 */
#define TURN_PHI 1024
static void turn(struct input *input, uint16_t *phi)
{
	if (input->x2 < 1024) {
		*phi += ((uint32_t) (1024 - input->x2) * (uint32_t) TURN_PHI) >> 10;
	}
	if (input->x2 > 2048)  {
		*phi -= ((uint32_t) (input->x2 - 2048) * (uint32_t) TURN_PHI) >> 10;
	}
}

/*
 * inputs
 *
 * @desc
 *   Handles all the input logic of the level
 */
#include "../cutscene/cutscene.h"
void level_inputs(struct input *input, uint16_t *x, uint16_t *y, uint16_t *phi, const uint8_t *world, uint16_t worldw)
{
	uint16_t thetam;
	*input = input_get();
	
	turn(input, phi);
	
	/* Move */
  if (input->y1 > INPUT_HIGH || input->x1 < INPUT_LOW || input->y1 < INPUT_LOW || input->x1 > INPUT_HIGH) {
    thetam = get_thetai(input, *phi) + *phi;

    uint8_t quad = 0;
    while (thetam > 0x4000) {
      thetam -= 0x4000;
      ++quad;
    }
    if (quad & 0x1) thetam = 16384 - thetam; /* Make sure theta is based off x-axis and not y */
    if (quad > 1) quad ^= 0x1; /* Convert quadrant to format specified above */

    move(input, x, y, thetam, quad, world, worldw);
  }
}