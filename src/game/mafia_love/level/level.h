#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

#define MAX_ENEMIES 10
#define MAX_PLAYER_BULLETS 5
/* Enemy bullets are shared across all enemies */
#define MAX_ENEMY_BULLETS 50
#define BULLET_SPAWN_THRESH 20
#define BOSS_TYPE_1 12
#define BOSS_TYPE_2 50

struct level {
	const uint8_t       *map;
	uint32_t             map_w; /* rizz */
	const struct sprite *enemies;
	uint8_t              enemy_count;
	uint16_t             spawn_x;
	uint16_t             spawn_y;
	uint16_t             spawn_phi;
};

struct sprite {
	uint16_t x;
	uint16_t y;
	uint8_t  id;
	uint16_t hp;
	uint32_t data; /* Used for enemies, bullet spawning */
	uint16_t dx;   /* Used for bullets */
	uint16_t dy;   /* Used for bullets */
	uint8_t  quad; /* Used for bullets */
};

void level_init(uint8_t level, uint8_t next_cutscene);
	
#endif