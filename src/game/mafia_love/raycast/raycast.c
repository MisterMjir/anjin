#include "raycast.h"
#include "world.h"
#include "sprites.h"

void rc_draw(uint16_t x, uint16_t y, uint16_t phi, const uint8_t *world, uint8_t worldw, 
	struct sprite *enemies, struct sprite *player_bullets, struct sprite *enemy_bullets)
{
	rc_draw_world(x, y, phi, world, worldw);
	rc_draw_sprites(x, y, phi, worldw, enemies, player_bullets, enemy_bullets);
}