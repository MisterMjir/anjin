#include "lose.h"
#include "../mgfx/mgfx.h"
#include "../mgfx/mgfxt.h"
#include "../input/input.h"
#include "../level/level.h"
#include "../sound/sound.h"
#include "core/core.h"

static uint8_t l;
static uint8_t c;

static uint32_t tick;

void lose_init_bruh(void);
void lose_init(uint8_t level, uint8_t cutscene)
{
	CORE_switch_state(lose_init_bruh);
	
	l = level;
	c = cutscene;
	
	sound_play(0x00);
}

#include "core/core.h"
#include "gl/gl_util.h"
// Empty demo for easy copy pasting

void lose_update(void)
{
	struct input input;

	input = input_get();

	if (input.buttons & INPUT_LTRG) {
		input_clear_button(INPUT_LTRG);
		if (l == 3) sound_play(0x01);
		else if (l == 2) sound_play(0x06);
		else sound_play(0x04);
		level_init(l, c);
	}
}

void lose_draw(void)
{
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mgfx_draw_rect(0, 0, WIN_W, WIN_H, 0);
	mgfx_send();
	mgfx_draw_sd(0, 0, 7);
	mgfxt_draw(0, 96, 160, "You lost, you got ratio'ed, but love always prevails. Shoot to try again", 2);
	mgfx_send();
}

void lose_quit(void)
{

}

void lose_init_bruh(void)
{
	CORE_foo.update_fn = lose_update;
	CORE_foo.draw_fn = lose_draw;
	CORE_foo.quit_fn = lose_quit;
}