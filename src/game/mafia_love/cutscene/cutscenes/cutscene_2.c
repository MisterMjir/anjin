#include "../../input/input.h"
#include "../../mgfx/mgfx.h"
#include "../../mgfx/mgfxt.h"
#include "../../level/level.h"
#include "../cutscene.h"
#include "../../sound/sound.h"

extern uint8_t language;

#define DTABLE_LEN 17
static struct dialogue dtable[] = {
	{3, 35, 0, 0, 90, 0xFF, "Yerraballer", {"Wow. It looks like there's more to life than just basketball.", "Guau. Parece que hay mas en la vida que solo baloncesto."}},
	{3, 35, 0, 0, 90, 0xEF, "Yerraballer", {"This love thing is really powerful. It's changed my whole perspective on life.", "Esto del amor es realmente poderoso. Ha cambiado toda mi perspectiva de la vida."}},
	{3, 35, 0, 0, 90, 0xEF, "Yerraballer", {"You know what, I think you need to show this to the Don.", "Sabes que, creo que necesitas mostrarle esto al Don."}},
	{3, 35, 0, 0, 90, 0xEF, "Yerraballer", {"His headquarters is right next to the TI lab.", "Su campo esta justo al lado del laboratorio de TI."}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"The TI lab? That's a few miles west of us.", "?El laboratorio de TI? Eso esta a unas pocas millas al oeste de nosotros"}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"Are you ready to take on the most dangerous man in this world?", "?Estas listo para enfrentarte al hombre mas peligroso de este mundo?"}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"The root of all heartbreaks and the crusher of all.", "La raiz de todos los desamores y la trituradora de todos."}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"I like you, kid-", "Me gustas, chico-"}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"*chugs some vodka*", "*bebe un poco de vodka*"}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"-you got a real heart. You really are bringing love to this world.", "tienes un corazon de verdad. Realmente estas trayendo amor a este mundo."}},
	{1, 35, 0, 0, 90, 0xEF, "McQueenski", {"Now let's go kid, we're gonna get the don.", "Ahora vamos chico, vamos a buscar al don."}},
	{4, 35, 0, 0, 90, 0xEF, "Don", {"How did you find me? You got past my right hand man? Impressive.", "?Como me encontraste? ?Pasaste a mi mano derecha? Impresionante"}},
	{4, 35, 0, 0, 90, 0xEF, "Don", {"Anyways this is where your journey ends.", "De todos modos, aqui es donde termina su viaje."}},
	{4, 35, 0, 0, 90, 0xEF, "Don", {"I am the most powerful force in the universe: my power level is over 9000.", "Soy la fuerza mas poderosa del universo: mi nivel de poder es superior a 9000."}},
	{4, 35, 0, 0, 90, 0xEF, "Don", {"I've created an empire, and you're not going to do anything about it.", "He creado un imperio y no vas a hacer nada al respecto."}},
	{5, 35, 0, 0, 90, 0x01, "Don", {"\0", "\0"}},
	{5, 35, 0, 0, 90, 0xEF, "Don", {"You're up against the boss.", "Estas contra el jefe"}},
};

#include "core/core.h"
#include "gl/gl_util.h"

static uint8_t i = 0;

void cutscene_2_update(void)
{
	struct input input;

	input = input_get();

	if (input.buttons & INPUT_LTRG) {
		++i;
		input_clear_button(INPUT_LTRG);

		if (i == DTABLE_LEN) {
			--i;
			level_init(3, CUTSCENE_3);
			return;
		}

		if (dtable[i].sound != 0xEF) sound_play(dtable[i].sound);
	}
}

void cutscene_2_draw(void)
{
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mgfx_draw_rect(0, 0, WIN_W, WIN_H, 0);
	mgfx_send();
	mgfxt_draw(0, 90, WIN_H, dtable[i].text[language], 2);
	mgfx_draw_sd(dtable[i].ix, dtable[i].iy, dtable[i].img);
	mgfx_send();
}

void cutscene_2_quit(void)
{

}

void cutscene_2_init(void)
{
	CORE_foo.update_fn = cutscene_2_update;
	CORE_foo.draw_fn = cutscene_2_draw;
	CORE_foo.quit_fn = cutscene_2_quit;

	if (dtable[i].sound != 0xEF) sound_play(dtable[i].sound);
}