#include "cutscene.h"
#include "../mgfx/mgfx.h"
#include "../mgfx/mgfxt.h"
#include "../input/input.h"
#include "../level/level.h"
#include "../sound/sound.h"
#include "core/core.h"

void cutscene_prologue_init(void);
void cutscene_1_init(void);
void cutscene_2_init(void);
void cutscene_3_init(void);

void (*cutscene_table[])(void) = {
	cutscene_prologue_init,
	cutscene_1_init,
	cutscene_2_init,
	cutscene_3_init
};

void cutscene_loop(void)
{
	/* Unused function */
	struct input input;
	
	mgfx_draw_rect(0, 0, WIN_W, WIN_H, 0);
	mgfx_send();
	mgfxt_draw(54, WIN_XMAX - 16, WIN_H - 2, "Kachow ;)", 3);
	mgfx_draw_sd(32, 0, 0);
	mgfx_send();
		
	while (!input_paused());
	input_clear_pause();
}

/*
 * cutscene_init
 *
 * @desc
 *   Switches the state to cutscene,
 *   and initializes the cutscene
 */
extern void (*loop_fn)(void);
void cutscene_init(uint8_t cutscene)
{
	CORE_switch_state(cutscene_table[cutscene]);
	
	sound_play(0x05);
}