#include "log/log.h"
#include "txt/txt.h"
#include "gl/gl_util.h"
#include "core/core.h"

void demo3_update(void)
{
  
}

void demo3_draw(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  TXT_draw("What's up bozo, I'm about to ratio you ;)", 25.0f, 25.0f, 0.75f);
}

void demo3_quit(void)
{
  
}

void demo3_init(void)
{
  CORE_foo.update_fn = demo3_update;
  CORE_foo.draw_fn = demo3_draw;
  CORE_foo.quit_fn = demo3_quit;
}