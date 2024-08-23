#include "sprites.h"
#include "raycast.h"
#include "../mmath/mmath.h"
#include "sprite_textures.h"

extern const uint16_t height_table[];
extern const uint16_t tex_step_table[];
void draw_sprite_strip(uint8_t screen_x, uint8_t sprite_x, uint16_t distance, uint8_t height, uint8_t id)
{
	uint8_t start_y;
	uint8_t color;
	const uint8_t *tex_row;
	uint16_t tex_step;       /* [15:8] integer, [7:0] fraction */
	uint16_t total_tex_step; /* [15:8] integer, [7:0] fraction */
	uint8_t  hit;            /* Check for completely blank vertical lines */
	
	start_y = ((WIN_W - height) >> 1);// + (WIN_W >> 2);
	tex_step = tex_step_table[height];
	tex_row = &sprite_textures[id][(((uint32_t) sprite_x * (uint32_t) tex_step) >> 8) * TEXTURE_SIZE];
	total_tex_step = 0;
	
	/* Draw */
	hit = 0;
	for (uint8_t i = start_y; i < start_y + height; ++i) {
		/* Determine which texel to use */
		color = tex_row[total_tex_step >> 8];

		if (color) {
			mgfx_draw_pixel(i, screen_x, color);
			hit = 1;
		}
		
		total_tex_step += tex_step;
	}
	
	/* Set z buffer */
	if (hit)
		rc_zbuffer[screen_x] = distance;
}

void draw_sprite_strips(uint16_t theta, uint8_t quad, uint16_t distance, uint8_t id)
{
	uint16_t screen_x; /* 'center' of the sprite */
	uint8_t height;
	uint16_t draw_x; /* Needs to be u16 like screen_x to prevent wrapping */
	uint8_t negative_x;
	
	if (distance >=  32769) return;
	else                    height = height_table[distance];
	if (height > 128) height = 128;
	
	if (theta > FOV_2) { /* Still possible to draw */
		negative_x = (theta - FOV_2) >> 6;
		if (negative_x > (height >> 1)) return;
		if (quad) screen_x = -negative_x;
		else      screen_x = WIN_YMAX + negative_x;
	}
	else {
		screen_x = (FOV_2 - theta) >> 6; /* >> 6 is / FOV_STEP */
		if (!quad) screen_x = WIN_YMAX - screen_x;
	}
	
	
	draw_x = screen_x - (height >> 1) - 1;
	for (uint8_t i = 0; i < height; ++i) {
		++draw_x;
		if (draw_x > WIN_YMAX) continue;
		if (rc_zbuffer[draw_x] < distance) continue;
		draw_sprite_strip(draw_x, i, distance, height, id);
	}
}

#define QPX 0x04
#define QPY 0x08
#define QTX 0x01
#define QTY 0x02
#define XCS 0x10
#define YSS 0x20
#define XSS 0x40
#define YCS 0x80
static uint8_t rotate_sprite(uint16_t px, uint16_t py, uint16_t phi, struct sprite *s, uint16_t *xout, uint16_t *yout, uint8_t *ysign, uint8_t pass)
{
	uint16_t x;
	uint16_t y;
	uint8_t quad; /* [7:6] yc, xs [5:4] ys, xc sign [3:2] player quad [1:0] theta quad, YX, 1 negative, 0 positive */
	uint16_t xc;  /* x * cosine thing */
	uint16_t ys;  /* y * sine thing */
	uint16_t xs;  /* x * sine thing */
	uint16_t yc;  /* y * cosine thing */
	
	x = s->x;
	y = s->y;
	
	/* Convert phi to theta prime */
	quad = 0;
	phi = 0 - phi;
	quad = mm_convert_theta(&phi);
	
	/* Translate to player (origin is now the player) */
	if (x < px) { quad |=  QPX; x = px - x; }
	else        { quad &= ~QPX; x = x - px; }
	if (y > py) { quad |=  QPY; y = y - py; }
	else        { quad &= ~QPY; y = py - y; }
	
	/* Set dx and dy for bullet spawning */
	if (!pass) {
		s->dx = x;
		s->dy = y;
		s->quad = quad;
	}
	
	/* Find matrix product stuff */
	xc = ((uint32_t) x * (uint32_t) mm_cost(phi)) >> 16;
	ys = ((uint32_t) y * (uint32_t) mm_sint(phi)) >> 16;
	quad = (quad & ~XCS) | ((((quad & QPX) >> 2) ^ ((quad & QTX) >> 0)) << 4);
	quad = (quad & ~YSS) | ((((quad & QPY) >> 3) ^ ((quad & QTY) >> 1)) << 5);
	xs = ((uint32_t) x * (uint32_t) mm_sint(phi)) >> 16;
	yc = ((uint32_t) y * (uint32_t) mm_cost(phi)) >> 16;
	quad = (quad & ~XSS) | ((((quad & QPX) >> 2) ^ ((quad & QTY) >> 1)) << 6);
	quad = (quad & ~YCS) | ((((quad & QPY) >> 3) ^ ((quad & QTX) >> 0)) << 7);
	
	/* Figure out x and cut off all negative xs (xc - ys) */
	if (quad & XCS) { /* Negative xc */		
		if (quad & YSS) { if (xc > ys) return 1; *xout = ys - xc; } /* -xc, -ys */
		else            { return 1; }                               /* -xc, +ys */
	}
	else {            /* Positive xc */
		if (quad & YSS) { *xout = xc + ys; }                        /* +xc, -ys */
		else            { if (ys > xc) return 1; *xout = xc - ys; } /* +xc, +ys */
	}
	
	/* Figure out y (xc + ys) */
	if (quad & XSS) { /* Negative xs */		
		if (quad & YCS) { *ysign = 1; *yout = xs + yc; } /* -xs, -yc */
		else {                                           /* -xs, +yc */
			if (yc > xs) {*ysign = 0; *yout = yc - xs; }
			else         {*ysign = 1; *yout = xs - yc; }
		}
	}
	else {            /* Positive xc */
		if (quad & YCS) {                                /* +xs, -yc */
			if (xs > yc) {*ysign = 0; *yout = xs - yc; }
			else         {*ysign = 1; *yout = yc - xs; }
		}                       
		else            { *ysign = 0; *yout = xs + yc; } /* +xs, +yc */
	}
	
	return 0;
}

static void draw_sprite(uint16_t px, uint16_t py, uint16_t phi, struct sprite *s, uint8_t worldw, uint8_t pass)
{
	uint16_t x;
	uint16_t y;
	uint8_t  quad; /* Really a flag for under player */
	uint16_t theta;
	uint16_t distance;
		
	if (rotate_sprite(px, py, phi, s, &x, &y, &quad, pass)) { if (!pass) s->data = 0; return; }
	
	theta = mm_atant(x, y);
	distance = ((uint32_t) mm_sect(theta) * (uint32_t) x) >> 8;
	
	/* Update sprite stuff for bullet spawning */
	if (!pass) {
		++s->data;
	}
	
	draw_sprite_strips(theta, quad, distance, s->id);
}

#define DRAW_LOOP while(sp->id) { draw_sprite(px, py, phi, sp++, worldw, pass); } ++pass;
void rc_draw_sprites(uint16_t px, uint16_t  py, uint16_t phi, uint8_t worldw,
	struct sprite *enemies, struct sprite *player_bullets, struct sprite *enemy_bullets)
{
	struct sprite *sp;
	uint8_t pass = 0;
	
	sp = enemies;
	DRAW_LOOP;
	sp = player_bullets;
	DRAW_LOOP;
	sp = enemy_bullets;
	DRAW_LOOP;
}