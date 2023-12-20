#include "log/log.h"
#include "txt/txt.h"
#include "gl/gl_util.h"
#include "core/core.h"
#include "gfx/gfx.h"
#include "space_catet.h"
#include "snd/snd.h"

static unsigned int txt;

void title_update(void)
{
  if (CORE_input.mouse.l == 1) {
    CORE_switch_state(level_init);
  }
}

void title_draw(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  GFX_draw_img(txt, 0, 0, 640, 480);
}

void title_quit(void)
{
  GL_texture_destroy(txt);
}

void title_init(void)
{
  CORE_foo.update_fn = title_update;
  CORE_foo.draw_fn = title_draw;
  CORE_foo.quit_fn = title_quit;

  CORE_window_title(CORE_wnd, "Space Catet");

  txt = GL_texture_create("res/space_catet/title.png");

  GFX_projection(0, 640, 480, 0, -1, 1);

  SND_bgm_set("res/space_catet/title.wav");
  SND_bgm_play();
}