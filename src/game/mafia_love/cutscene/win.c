#include "win.h"
#include "../mgfx/mgfx.h"
#include "../mgfx/mgfxt.h"
#include "cutscene.h"
#include "../sound/sound.h"
#include "core/core.h"

void win_init_bruh(void);
void win_init(void)
{
	CORE_switch_state(win_init_bruh);
}

#include "core/core.h"
#include "gl/gl_util.h"
// Empty demo for easy copy pasting

void win_update(void)
{

}

void win_draw(void)
{
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mgfx_draw_rect(0, 0, WIN_W, WIN_H, 0);
	mgfx_send();
	mgfx_draw_sd(0, 0, 9);
	mgfx_send();
}

void win_quit(void)
{

}

void win_init_bruh(void)
{
	CORE_foo.update_fn = win_update;
	CORE_foo.draw_fn = win_draw;
	CORE_foo.quit_fn = win_quit;

	sound_play(0x03);
}