#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdint.h>
#include "../mgfx/mgfx.h"
#include "../level/level.h"

/*
 * The big boi
 */
 
/* Field of view is ~60 degrees, changed it so FOV_STEP is a power of 2 */
#define FOV 10240
#define FOV_2 5120
#define FOV_STEP 64

#define LVL_MAX_SIZE 4096 /* 64 x 64 square */

extern uint16_t rc_zbuffer[WIN_H];
 
void rc_draw(uint16_t x, uint16_t y, uint16_t phi, const uint8_t *world, uint8_t worldw,
	struct sprite *enemies, struct sprite *player_bullets, struct sprite *enemy_bullets);

#endif