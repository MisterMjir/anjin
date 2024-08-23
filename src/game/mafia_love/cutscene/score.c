#include "score.h"
#include "../mgfx/mgfx.h"
#include "../mgfx/mgfxt.h"
#include "../input/input.h"
#include "../cutscene/cutscene.h"
#include "core/core.h"

uint16_t score_time;
uint16_t score_hp;
uint16_t score_total;
uint8_t  c;

static char score_time_text[7];
static char score_hp_text[7];
static char score_total_text[7];

void num_to_str(uint16_t num, char *str)
{
	str[6] = '\0';
	
	for (int8_t i = 5; i >= 0; --i) {
		str[i] = (num % 10) + '0';
		num /= 10;
	}
}

void score_init_bruh(void);
void score_init(uint32_t time, uint16_t hp, uint8_t cutscene)
{
	CORE_switch_state(score_init_bruh);
	
	c = cutscene;
	
	score_time = time;
	score_hp = hp;
	score_total = time + hp;
	
	num_to_str(score_time, score_time_text);
	num_to_str(score_hp, score_hp_text);
	num_to_str(score_total, score_total_text);
}

#include "core/core.h"
#include "gl/gl_util.h"
// Empty demo for easy copy pasting

void score_update(void)
{
	struct input input;

	input = input_get();

	if (input.buttons & INPUT_LTRG) {
		input_clear_button(INPUT_LTRG);
		cutscene_init(c);
	}
}

void score_draw(void)
{
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mgfx_draw_rect(0, 0, WIN_W, WIN_H, 0);
	mgfxt_draw(0, 0, 160, "Congragulations, you successfuly spread love to the world, here's how you did.", 2);
	/* Nine spaces */
	mgfxt_draw(4, 32, 160, "Time:   ", 2);
	mgfxt_draw(4 + 9 * 6, 32, 160, score_time_text, 2);
	mgfxt_draw(4, 48, 160, "Health: ", 2);
	mgfxt_draw(4 + 9 * 6, 48, 160, score_hp_text, 2);
	mgfxt_draw(4, 64, 160, "Total:  ", 2);
	mgfxt_draw(4 + 9 * 6, 64, 160, score_total_text, 2);
	mgfx_send();
}

void score_quit(void)
{

}

void score_init_bruh(void)
{
	CORE_foo.update_fn = score_update;
	CORE_foo.draw_fn = score_draw;
	CORE_foo.quit_fn = score_quit;
}