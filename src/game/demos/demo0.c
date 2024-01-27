#include "core/core.h"
#include "gl/gl_util.h"
// Empty demo for easy copy pasting

void demo0_update(void)
{

}

void demo0_draw(void)
{
  glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void demo0_quit(void)
{
  
}

void demo0_init(void)
{
  CORE_foo.update_fn = demo0_update;
  CORE_foo.draw_fn = demo0_draw;
  CORE_foo.quit_fn = demo0_quit;
}