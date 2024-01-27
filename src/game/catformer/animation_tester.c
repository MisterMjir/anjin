#include "core/core.h"
#include "gfx/gfx.h"
#include "gl/gl_util.h"

enum player_state {
  IDLE,
  WALKING
};

struct player {
  enum player_state state;
  unsigned int      texture;
  unsigned int      ticks;
  unsigned int      sub_state;
  int               tex_x;
};

static struct player player;

void level_update(void)
{
  ++player.ticks;

  if (player.ticks % 8 == 0) {
    ++player.sub_state;
    if (player.sub_state > 3) player.sub_state = 0;
    switch (player.sub_state) {
      case 0: player.tex_x = 0; break;
      case 1: player.tex_x = 80; break;
      case 2: player.tex_x = 0; break;
      case 3: player.tex_x = 160; break;
    }
  }
}

void level_draw(void)
{
  glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GFX_draw_imgx(player.texture, 200, 64, 160, 256, 0 + 80 * player.tex_x, 0, 80, 128, 1, 0);
}

void level_quit(void)
{
  
}

void level_init(void)
{
  CORE_foo.update_fn = level_update;
  CORE_foo.draw_fn = level_draw;
  CORE_foo.quit_fn = level_quit;

  player.state = WALKING;
  player.ticks = 0;
  player.sub_state = 0;
  player.tex_x = 0;

  player.texture = GL_texture_create("res/catformer/plsfrogiveme.png");

  GFX_projection(0, 640, 480, 0, -1, 1);
}