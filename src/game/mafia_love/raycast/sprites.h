#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>
#include "../level/level.h"

void rc_draw_sprites(uint16_t px, uint16_t  py, uint16_t phi, uint8_t worldw,
	struct sprite *enemies, struct sprite *player_bullets, struct sprite *enemy_bullets);


#endif