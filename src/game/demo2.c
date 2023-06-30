#include "log/log.h"
#include "gl/gl_util.h"
#include "gfx/gfx.h"
#include <stdint.h>
#include "core/core.h"

static unsigned int texture;
static uint16_t r, g, b;
static int dirr, dirg, dirb;

void demo2_update(void)
{
  if (dirr) {if (!++r) dirr = 0;}
  else      {if (!--r) dirr = 1;}

  if (dirg) {if (!++g) dirg = 0;}
  else      {if (!--g) dirg = 1;}

  if (dirb) {if (!++b) dirb = 0;}
  else      {if (!--b) dirb = 1;}
}

void demo2_draw(void)
{
  glClearColor(r / 65535.0, g / 65535.0, b / 65535.0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GFX_draw_img(texture, 0, 0, 256, 256);
}

void demo2_quit(void)
{
  GL_texture_destroy(texture);
}

void demo2_init(void)
{
  CORE_foo.update_fn = demo2_update;
  CORE_foo.draw_fn = demo2_draw;
  CORE_foo.quit_fn = demo2_quit;

  texture = GL_texture_create("res/img/test_image.png");
  GFX_projection(0, 640, 480, 0, -1, 1);

  r = 0;
  g = 21845;
  b = 43690;
  dirr = dirg = dirb = 1;
}