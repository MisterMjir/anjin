#include "init.h"

#include "core/core.h"
#include "gl/gl_util.h"

/* Inits the game to the title state */
#include "cutscene/title.h"

void (*loop_fn) (void);

void mafia_update(void)
{
	CORE_switch_state(title_init);
}

void mafia_draw(void)
{
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mafia_quit(void)
{

}

#include "mgfx/mgfx.h"
#include "sound/sound.h"
void mafia_init(void)
{
	CORE_foo.update_fn = mafia_update;
	CORE_foo.draw_fn = mafia_draw;
	CORE_foo.quit_fn = mafia_quit;

	mgfx_init();
	sound_init();
}