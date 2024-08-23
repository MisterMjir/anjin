#include "title.h"
#include "../mgfx/mgfx.h"
#include "../mgfx/mgfxt.h"
#include "../input/input.h"
#include "cutscene.h"
#include "../sound/sound.h"

uint8_t language;

#include "core/core.h"
#include "gl/gl_util.h"
// Empty demo for easy copy pasting

void title_update(void)
{
	struct input input;
	input = input_get();

	if (input.buttons & (INPUT_LTRG | INPUT_RTRG)) {
		if (input.buttons & INPUT_RTRG) {
			language = 1;
		}
		input_clear_button(INPUT_LTRG | INPUT_RTRG);
		cutscene_init(CUTSCENE_PROLOGUE);
	}
}

void title_draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mgfx_draw_sd(0, 0, 0);
	mgfx_send();
}

void title_quit(void)
{

}

void title_init(void)
{
	CORE_foo.update_fn = title_update;
	CORE_foo.draw_fn = title_draw;
	CORE_foo.quit_fn = title_quit;

	sound_play(0x05);
}