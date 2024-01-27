#include "core.h"
#include <time.h>
#include "snd/snd.h"
#include "gl/gl_util.h"
#include "gfx/gfx.h"
#include "txt/txt.h"
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

struct CORE_foo     CORE_foo;
struct CORE_window *CORE_wnd;

static void dummy_fn(void) {}

static int init(void)
{
  if (GL_init()) return -1;
  GL_window_init(CORE_wnd);

  if (SND_init()) return -1;

  GL_set(CORE_wnd); /* Context must be set in the thread its used in */

  GFX_init();
  //TXT_init();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return 0;
}

static void quit(void)
{
  TXT_quit();
  GFX_quit();

  GL_unset(CORE_wnd);

  SND_quit();
}

static void window_resize(int w, int h)
{
  CORE_foo.win_w = w;
  CORE_foo.win_h = h;
  CORE_foo.resize = 1;
}

/**
 * CORE_init
 *
 * Initializes everything CORE needs
 * 
 * @param fps Max FPS
 */
#define INIT_FN(fn) if ((res = fn)) return res;
int CORE_init(int w, int h, unsigned int fps, void (*init_fn)(void))
{
  int res;

  CORE_set_fps(fps);
  CORE_switch_state(init_fn);
  CORE_foo.quit_fn = dummy_fn;
  CORE_foo.update_fn = dummy_fn;
  CORE_foo.draw_fn = dummy_fn;
  CORE_foo.win_w = w;
  CORE_foo.win_h = h;
  CORE_foo.resize = 0;

  res = 0;
  INIT_FN(CORE_window_init());

  CORE_wnd = CORE_window_create(w, h);
  CORE_window_cb_size = window_resize;
  CORE_window_icon(CORE_wnd, "res/core/favicon.ico");
  CORE_window_title(CORE_wnd, "anjin app");

  return res;
}

/**
 * CORE_input_loop
 *
 * Free up all resources used by CORE
 */
void CORE_quit(void)
{
  CORE_window_destroy(CORE_wnd);
}

/**
 * CORE_game_loop
 *
 * The game loop
 *
 * @param data Dummy parameter, needed for threads
 */
void CORE_game_tick(void)
{
  CORE_input_update();

  if (CORE_foo.resize) {
    glViewport(0, 0, CORE_foo.win_w, CORE_foo.win_h);
    CORE_foo.resize = 0;
  }

  if (CORE_foo.init) {
    CORE_foo.quit_fn();
    CORE_foo.init_fn();
    CORE_foo.init = 0;
  }
  
  CORE_foo.update_fn();

  CORE_foo.draw_fn();
  CORE_window_swap_buffers(CORE_wnd);
}

THREAD_FN CORE_game_loop(void *data)
{
  clock_t ts, te; /* Tick start, tick end */
  double  tt;     /* Tick time */

  init();

  #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(CORE_game_tick, CORE_foo.fps, EM_TRUE);
  #else
    while (1) {
      ts = clock();

      CORE_game_tick();
      if (CORE_input.quit) break;

      te = clock();

      tt = (te - ts) / (double) CLOCKS_PER_SEC * 1000;
      if (tt < CORE_foo.td) {
        CORE_sleep(CORE_foo.td - tt);
      }
    }
  #endif

  quit();

  THREAD_RETURN;
}

void CORE_set_fps(unsigned int fps)
{
  CORE_foo.fps = fps;
  CORE_foo.td = 1000.0 / fps;
}

/**
 * CORE_switch_state
 *
 * Comment the first line to use the latest state
 * With no comment, if there are multiples calls to this
 * function, it will use the first instance of the call
 */
void CORE_switch_state(void (*init_fn)(void))
{
  if (CORE_foo.init) return;
  CORE_foo.init_fn = init_fn;
  CORE_foo.init    = 1;
}