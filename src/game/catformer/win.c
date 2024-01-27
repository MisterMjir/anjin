#include "gl/gl_util.h"
#include "core/core.h"
#include "gfx/gfx.h"
#include "catformer.h"
#include "snd/snd.h"

static unsigned int txt;

void win_update(void)
{
  if (CORE_input.mouse.l == 1) {
    CORE_switch_state(title_init);
  }
}

void win_draw(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  GFX_draw_img(txt, 0, 0, 640, 480);
}

void win_quit(void)
{
  GL_texture_destroy(txt);
}

void win_init(void)
{
  CORE_foo.update_fn = win_update;
  CORE_foo.draw_fn = win_draw;
  CORE_foo.quit_fn = win_quit;

  txt = GL_texture_create("res/catformer/win.png");

  GFX_projection(0, 640, 480, 0, -1, 1);

  SND_bgm_set("res/catformer/win.wav");
  SND_bgm_play();
}