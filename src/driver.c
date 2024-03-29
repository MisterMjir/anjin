#include "core/core.h"
#include "snd/snd.h"
#include "game/demos/demos.h"
//#include "game/space_catet/space_catet.h"
#include "game/catformer/catformer.h"

#define WIN_W 640
#define WIN_H 480
#define FPS 60
#define FIRST_INIT title_init
//#define FIRST_INIT demo2_init
//#define FIRST_INIT level_init

#ifndef __EMSCRIPTEN__

int main(int argc, char *args[])
{
  struct CORE_thread *g;
  struct CORE_thread *s;

  CORE_init(WIN_W, WIN_H, FPS, FIRST_INIT);

  if (!(g = CORE_thread_create(CORE_game_loop))) return -1;
  if (!(s = CORE_thread_create(SND_loop))) return -1;

  CORE_input_loop();
  CORE_thread_join(g);
  CORE_thread_join(s);

  CORE_thread_destroy(g);
  CORE_thread_destroy(s);

  CORE_quit();

  return 0;
}

#else

int main(int argc, char *args[])
{
  struct CORE_thread *s;

  CORE_init(WIN_W, WIN_H, FPS, FIRST_INIT);

  if (!(s = CORE_thread_create(SND_loop))) return -1;

  CORE_input_loop(); /* For emscripten this isn't a loop */
  CORE_game_loop(NULL); /* Needs to be on main thread to access DOM */
  CORE_thread_join(s);

  CORE_thread_destroy(s);

  CORE_quit();

  return 0;
}

#endif