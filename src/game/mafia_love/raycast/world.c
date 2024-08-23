#include "world.h"
#include "raycast.h"
#include "../mmath/mmath.h"

/*
 * References
 * https://lodev.org/cgtutor/raycasting.html
 * https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/
 */

/* The screen is rotated, so it's a z buffer for x but there is y amount of x */
uint16_t rc_zbuffer[WIN_H];

/*
 * draw_world_strip
 *
 * @desc
 *   Draws a strip (vertical line)
 * @param screen_x
 *   Where to draw the strip
 * @param distance
 *   Distance of the wall, what else can I say?
 * @param data
 *   Upper byte - Logic 0 for x hit, logic 1 for y hit
 *   Lower byte - Fractional part of hit on the wall
 * @param tile
 *   Which tile was hit (basically what texture to use)
 */
#include "height_table.h"
#include "tex_step_table.h"
#include "../mgfx/textures.h"
/*
 * draw_world_strip
 *
 * @desc
 *   Draw a world 'strip', a single vertical line
 * @param screen_x
 *   The x position on the screen
 * @param distance
 *   How far away the wall is
 * @param data
 *   [] idk, [7:0] tile offset (fractional part of tile hit)
 * @param tile
 *   Which tile was hit (id, not array value)
 */
void rc_draw_world_strip(uint8_t screen_x, uint16_t distance, uint16_t data, uint8_t tile)
{
	uint8_t color;
	const uint8_t *tex_row;
	uint8_t start_y;        /* Where to start drawing the strip */
	uint8_t height;
	uint16_t tex_step;       /* [15:8] integer, [7:0] fraction */
	uint16_t total_tex_step; /* [15:8] integer, [7:0] fraction */
		
	/* Initialize */
	if (distance >=  32769) return;
	else                    height = height_table[distance];
	
	start_y = (WIN_W - height) >> 1;

	tex_row = &textures[tile][((data & 0x00FF) >> 2) * TEXTURE_SIZE]; /* >> 2 to go from 256 -> 64 */
	tex_step = tex_step_table[height];
	total_tex_step = 0;
	
	
	/* Draw */
	for (uint8_t i = start_y; i < start_y + height; ++i) {
		/* Determine which texel to use */
		color = tex_row[total_tex_step >> 8];
		if (!(data >> 8)) color += 32;

		mgfx_draw_pixel(i, screen_x, color);
		
		total_tex_step += tex_step;
	}
	
	/* Set z buffer */
	rc_zbuffer[screen_x] = distance;
}

#define QX (quadrant & 0x1)
#define QY (quadrant & 0x2)
/* Find distance of vertical wall hit */
uint16_t rc_find_ri(uint16_t x, uint16_t y, uint16_t theta, uint8_t quadrant, const uint8_t *world, uint8_t worldw, uint16_t *tile, uint8_t *to)
{
  uint16_t first_step;
  uint16_t y_step;
  uint16_t x_step;
  uint16_t old_x;

  old_x = x;
  y_step = mm_tant(theta);

  /* Shift to starting point */
  if (QX) {
		first_step = ((uint32_t) y_step * (uint32_t) (x & 0xFF)) >> 8;
		x_step = 0xFF00;
		x &= 0xFF00;
  }
  else {
		first_step = ((uint32_t) y_step * (uint32_t) (0x0100 - (x & 0xFF))) >> 8;
		x += 0x0100;
		x_step = 0x0100;
		x &= 0xFF00;
  }
  if (!QY) { y_step = 0 - y_step; first_step = 0 - first_step; }
  y += first_step;

  /* Loop until wall is hit */
  while (!(*tile = world[(y >> 8) * worldw + (x >> 8) - (QX)])) {
		x += x_step;
		y += y_step;
		if ((y >> 8) >= worldw) {
			*tile = 1;
			return 0xFFFF;
		}
  }

  *to = (y & 0xFF);

  if (QX) return ((uint32_t) mm_sect(theta) * (uint32_t) (old_x - x)) >> 8;
  else    return ((uint32_t) mm_sect(theta) * (uint32_t) (x - old_x)) >> 8;
}

/* Find distance of horizontal wall hit */
uint16_t rc_find_rj(uint16_t x, uint16_t y, uint16_t theta, uint8_t quadrant, const uint8_t *world, uint8_t worldw, uint16_t *tile, uint8_t *to)
{
  uint16_t first_step;
  uint16_t x_step;
  uint16_t y_step;
  uint16_t old_y;

  old_y = y;
  x_step = mm_cott(theta);

  /* Shift to starting point */
  if (QY) {
		first_step = ((uint32_t) x_step * (uint32_t) (0x0100 - (y & 0xFF))) >> 8;
		y_step = 0x0100;
		y &= 0xFF00;
		y += 0x0100;
  }
  else {
		first_step = ((uint32_t) x_step * (uint32_t) (y & 0xFF)) >> 8;
		y_step = 0xFF00;
		y &= 0xFF00;
  }
  if (QX) { x_step = 0 - x_step; first_step = 0 - first_step; }
  x += first_step;

  /* Injected */
  uint16_t start_x = x, start_y = y;

  /* Loop until wall is hit */
  if (QY) {
		while (!(*tile = world[(y >> 8) * worldw + (x >> 8)])) {
			x += x_step;
			y += y_step;
			if ((x >> 8) >= worldw) {
				*tile = 1;
				return 0xFFFF;
			}
		}
  }
  else {
		while (!(*tile = world[((y - 0x0100) >> 8) * worldw + (x >> 8)])) {
			x += x_step;
			y += y_step;
			if ((x >> 8) >= worldw) {
				*tile = 1;
				return 0xFFFF;
			}
		}
  }

  *to = (x & 0xFF);

  if (QY) return ((uint32_t) mm_csct(theta) * (uint32_t) (y - old_y)) >> 8;
  else    return ((uint32_t) mm_csct(theta) * (uint32_t) (old_y - y)) >> 8;
}

/*
 * draw_world_ray
 *
 * @desc
 *   Cast a single ray and draw it
 * @param screen_x
 *   Where the ray will be drawn on the screen
 * @param x
 *   Player x (fixed [15:8] int [7:0] fraction)
 * @param y
 *   Player y (fixed [15:8] int [7:0] fraction)
 * @param theta
 *   Ray angle (units of 2 * pi / 2^16)
 * @param phi
 *   Player angle (units of 2 * pi / 2^16)
 * @param world
 *   World/level array
 * @param worldw
 *   How many columns are in the world
 */
void rc_draw_world_ray(uint8_t screen_x, uint16_t x, uint16_t y, uint16_t theta, uint16_t phi, const uint8_t* world, uint8_t worldw)
{
  uint16_t ri;
  uint16_t rj;
  uint16_t distance;
  uint16_t tilei, tilej, tile;
  uint8_t toi, toj, to;
  uint16_t data;

  /* Turn phi into difference between theta and phi */
  phi = theta - phi;
  if (phi > FOV_2) phi = 0 - phi;

  /* Convert theta */
	data = mm_convert_theta(&theta);

  ri = rc_find_ri(x, y, theta, data & 0xFF, world, worldw, &tilei, &toi);
  rj = rc_find_rj(x, y, theta, data & 0xFF, world, worldw, &tilej, &toj);

  if (ri < rj) { distance = ri; tile = tilei; to = toi; }
  else		     { distance = rj; tile = tilej; to = toj; data |= 0x8000; }

  /* Correct distance */
  distance = ((uint32_t) distance * mm_cost(phi)) >> 16;

  data = (data & ~0xFF) | to;
  rc_draw_world_strip(screen_x, distance, data, tile);
}

/*
 * draw_world
 *
 * @desc
 *   Renders the world, meaning all tiles and no sprites,
 *   but does set the z buffer
 * @param x
 *   Player x (fixed [15:8] int [7:0] fraction)
 * @param y
 *   Player y (fixed [15:8] int [7:0] fraction)
 * @param phi
 *   Player angle (units of 2 * pi / 2^16)
 * @param world
 *   World/level array
 * @param worldw
 *   How many columns are in the world
 */
void rc_draw_world(uint16_t x, uint16_t y, uint16_t phi, const uint8_t *world, uint8_t worldw)
{
	/* Cast a ray for every x position */
	uint8_t i;
	uint16_t angle;
	for (i = 0, angle = phi - FOV_2; i < WIN_H; ++i, angle += FOV_STEP) {
		rc_draw_world_ray(i, x, y, angle, phi, world, worldw);
	}
}