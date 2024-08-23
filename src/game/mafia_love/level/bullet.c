#include "bullet.h"
#include "../mmath/mmath.h"
#include "../sound/sound.h"
#include "../cutscene/score.h"
#include "../cutscene/lose.h"

/* Side length from middle */
#define WIDTH_2 0x0010
#define WIDTH   0x0021
#define SPRITE_WIDTH_2 0x0040
#define SPRITE_WIDTH   0x0081

extern struct level world_table[];

extern uint8_t       current_world;
extern uint16_t      x;
extern uint16_t      y;
extern uint16_t      phi;
extern uint16_t      hp;
extern uint32_t      time_ml;
extern uint8_t       next_cutscene;

extern struct sprite enemies[MAX_ENEMIES + 1];
extern uint8_t       enemy_count;
extern struct sprite player_bullets[MAX_PLAYER_BULLETS + 1];
extern uint8_t       player_bullet_count;
extern struct sprite enemy_bullets[MAX_ENEMY_BULLETS + 1];
extern uint8_t       enemy_bullet_count;

extern uint16_t output_ammo;
extern uint16_t output_ammo_step;
extern uint16_t output_hp;
extern uint16_t output_hp_step;
extern uint16_t output_ehp;
extern uint16_t output_ehp_step;

/*
 * sprite_collision
 */
uint8_t bullet_sprite_collision(struct sprite *b, uint16_t x, uint16_t y)
{
	uint16_t bx;
	uint16_t by;
	uint16_t sx;
	uint16_t sy;
	
	bx = b->x - WIDTH_2;
	by = b->y - WIDTH_2;
	sx = x - SPRITE_WIDTH_2;
	sy = y - SPRITE_WIDTH_2;
	
	if (bx < sx + SPRITE_WIDTH &&
			bx + WIDTH > sx &&
			by < sy + SPRITE_WIDTH &&
			by + WIDTH > sy) {
		return 1;
	}
	
	return 0;
}

/*
 * destroy
 */
void bullet_destroy(uint8_t i, struct sprite *arr, uint8_t *count)
{
	arr[i].x     = arr[*count - 1].x;
	arr[i].y     = arr[*count - 1].y;
	arr[i].id    = arr[*count - 1].id;
	arr[i].hp    = arr[*count - 1].hp;
	arr[i].data  = arr[*count - 1].data;
	arr[i].dx    = arr[*count - 1].dx;
	arr[i].dy    = arr[*count - 1].dy;
	arr[i].quad  = arr[*count - 1].quad;
	
	--*count;
	arr[*count].id = 0;
}

/*
 * create
 */
uint8_t bullet_create(uint8_t id, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t hp, uint8_t quad, struct sprite *arr, uint8_t *count, uint8_t max)
{
	uint8_t i;
	
	i = *count;
	if (i == max) return 1;
	
	arr[i].x = x;
	arr[i].y = y;
	arr[i].id = id;
	arr[i].hp = hp;
	arr[i].dx = dx;
	arr[i].dy = dy;
	arr[i].quad = quad;
	
	++*count;
	
	arr[*count].id = 0;
	
	return 0;
}

/*
 * update_player_bullets
 */
#define R 0x0030
void update_player_bullets(struct input *input)
{
	uint16_t bx;
	uint16_t by;
	uint16_t dx;
	uint16_t dy;
	uint16_t bhp;
	uint8_t  quad;
	uint16_t theta;
	uint16_t damage;
	
	/* Spawn player bullet */
	if (input->buttons & INPUT_LTRG) {
		input_clear_button(INPUT_LTRG);
		bhp = -1;
		theta = phi;
		quad = mm_convert_theta(&theta);
		dx = MM_COS_MUL(theta, R);
		dy = MM_SIN_MUL(theta, R);
		/* Move the bullet a little away from the player (HACK) */
		#define DX_MUL 5
		if (quad & 0x1)
			bx = x - dx * DX_MUL;
		else
			bx = x + dx * DX_MUL;
		if (quad & 0x2)
			by = y + dy * DX_MUL;
		else
			by = y - dy * DX_MUL;
		
		if (!bullet_create(3, bx, by, dx, dy, bhp, quad, player_bullets, &player_bullet_count, MAX_PLAYER_BULLETS)) {
			sound_play(0x80);
		}
	}
	/* Player bullet update */
	for (uint8_t i = 0; i < player_bullet_count; ++i) {
		quad = player_bullets[i].quad;
		/* Move */
		if (quad & 0x1)
			bx = player_bullets[i].x -= player_bullets[i].dx;
		else
			bx = player_bullets[i].x += player_bullets[i].dx;
		if (quad & 0x2)
			by = player_bullets[i].y += player_bullets[i].dy;
		else
			by = player_bullets[i].y -= player_bullets[i].dy;
		
		/* Max distance */
		if (!--player_bullets[i].hp) {
			bullet_destroy(i, player_bullets, &player_bullet_count);
			--i;
		}
		/* Wall collision */
		if (world_table[current_world].map[(by >> 8) * world_table[current_world].map_w + (bx >> 8)]) {
			bullet_destroy(i, player_bullets, &player_bullet_count);
			--i;
		}
		/* Sprite collision */
		for (uint8_t j = 0; j < enemy_count; ++j) {
			if (bullet_sprite_collision(player_bullets + i, enemies[j].x, enemies[j].y)) {
				/* Destroy bullet */
				bullet_destroy(i, player_bullets, &player_bullet_count);
				--i;
				damage = 10;
				/* Damage enemy */
				output_ehp -= output_ehp_step * 10;
				if (damage >= enemies[j].hp) { /* Defeated enemy */
					bullet_destroy(j, enemies, &enemy_count);
					if (!enemy_count) { /* Beat the level */
						score_init(time_ml, hp, next_cutscene);
					}
					if (enemies[j].id == 1) {
						sound_play(0x82);
					}
					if (enemies[j].id == 4) {
						sound_play(0x81);
					}
				}
				else { /* Enemy is still alive */
					enemies[j].hp -= damage;
				}
				break;
			}
		}
	}
}

/*
 * enemy_bullet_dx_dy
 */
void enemy_bullet_dx_dy(struct sprite *s, uint16_t *dx, uint16_t *dy, uint16_t r)
{
	uint16_t theta;
	uint16_t x;
	uint16_t y;
	
	x = s->dx;
	y = s->dy;
	
	if (x == 0) { /* Prevent divide by 0 apocalypse */
		*dx = 0;
		*dy = r;
		return;
	}
	
	theta = mm_atant(x, y);
	
	*dx = MM_COS_MUL(theta, r);
	*dy = MM_SIN_MUL(theta, r);
}

/*
 * update_enemy_bullets
 */
void update_enemy_bullets(void)
{
	uint16_t bx;
	uint16_t by;
	uint8_t  id;
	uint16_t bhp;
	uint16_t theta;
	uint16_t dx;
	uint16_t dy;
	uint8_t  quad;
	uint16_t damage;
	/* Enemy bullet spawning */
	for (uint8_t i = 0; i < enemy_count; ++i) {
		switch (enemies[i].id) {
			case 10:
			case 11:
			case 12:
				if ((enemies[i].data & 0x03) == 0) {
					if (enemies[i].id == 12) enemies[i].id = 10;
					else ++enemies[i].id;
				}
			break;
			case 6:
				if ((enemies[i].data & 0x3) == 0) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					bhp = -1;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0040);
					bullet_create(id, bx, by, dx, dy, bhp, 0, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
					bullet_create(id, bx, by, dx, dy, bhp, 1, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
					bullet_create(id, bx, by, dx, dy, bhp, 2, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
					bullet_create(id, bx, by, dx, dy, bhp, 3, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
				}
				if (enemies[i].data == BOSS_TYPE_2) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					quad = enemies[i].quad;
					bhp = -1;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0080);
					bullet_create(id, bx, by, dx, dy, bhp, quad, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
				}
				
				if (enemies[i].data == BOSS_TYPE_2 + 4) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					quad = enemies[i].quad;
					bhp = -1;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0040);
					bullet_create(id, bx, by, dx, dy, bhp, quad, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
				}
				if (enemies[i].data == BOSS_TYPE_2 + 8) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					quad = enemies[i].quad;
					bhp = -1;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0040);
					bullet_create(id, bx, by, dx, dy, bhp, quad, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
				}
				if (enemies[i].data == BOSS_TYPE_2 + 12) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					quad = enemies[i].quad;
					bhp = -1;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0040);
					bullet_create(id, bx, by, dx, dy, bhp, quad, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS);
				}
				
				if (enemies[i].data > BOSS_TYPE_2 + 12) {
					enemies[i].data = 0;
				}
			break;
			default:
				if (enemies[i].data == BULLET_SPAWN_THRESH) {
					bx = enemies[i].x;
					by = enemies[i].y;
					id = enemies[i].id + 1;
					bhp = -1;
					quad = enemies[i].quad;
					enemy_bullet_dx_dy(enemies + i, &dx, &dy, 0x0020);
					if (bullet_create(id, bx, by, dx, dy, bhp, quad, enemy_bullets, &enemy_bullet_count, MAX_ENEMY_BULLETS)) enemies[i].data -= 1;
					else                                                                                                     enemies[i].data = 0;
				}
				if (enemies[i].data > BULLET_SPAWN_THRESH) {
					enemies[i].data = 0;
				}
			break;
		}
	}
	
	/* Enemy bullet update */
	for (uint8_t i = 0; i < enemy_bullet_count; ++i) {
		quad = enemy_bullets[i].quad;
		/* Move */
		if (quad & 0x1)
			bx = enemy_bullets[i].x += enemy_bullets[i].dx;
		else
			bx = enemy_bullets[i].x -= enemy_bullets[i].dx;
		if (quad & 0x2)
			by = enemy_bullets[i].y += enemy_bullets[i].dy;
		else
			by = enemy_bullets[i].y -= enemy_bullets[i].dy;
		
		/* Max distance */
		if (!--enemy_bullets[i].hp) {
			bullet_destroy(i, enemy_bullets, &enemy_bullet_count);
			--i;
		}
		/* Wall collision */
		if (world_table[current_world].map[(by >> 8) * world_table[current_world].map_w + (bx >> 8)]) {
			bullet_destroy(i, enemy_bullets, &enemy_bullet_count);
			--i;
		}
		/* Player collision */
		if (bullet_sprite_collision(enemy_bullets + i, x, y)) {
			/* Destroy bullet */
				bullet_destroy(i, enemy_bullets, &enemy_bullet_count);
				--i;
				damage = 20;
				/* Damage Player */
				if (damage >= hp) { /* Defeated player */
					lose_init(current_world, next_cutscene);
				}
				else { /* Enemy is still alive */
					hp -= damage;
				}
				break;
		}
	}
}

/*
 * update_bullets
 */
void update_bullets(struct input *input)
{
	update_player_bullets(input);
	update_enemy_bullets();
}