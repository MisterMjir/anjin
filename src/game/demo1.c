#include "core/core.h"
#include "snd/snd.h"
#include <stdio.h>
#include "gl/gl_util.h"

static unsigned int shader;
static unsigned int VAO;

void demo1_update(void)
{
  if (CORE_input.keys.space == 1) {
    if (SND_bgm_state()) SND_bgm_pause();
    else                 SND_bgm_play();
  }

  if (CORE_input.keys.n1 == 1) {
    SND_bgm_set("res/music/letter.wav");
    SND_bgm_play();
  }
  if (CORE_input.keys.n2 == 1) {
    SND_bgm_set("res/music/sad_song.wav");
    SND_bgm_play();
  }

  if (CORE_input.keys.f11 == 1) {
    CORE_window_fullscreen(CORE_wnd, 1);
  }

  if (CORE_input.keys.escape == 1) {
    CORE_window_fullscreen(CORE_wnd, 0);
  }
}

void demo1_draw(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void demo1_quit(void)
{
  GL_shader_destroy(shader);
}

void demo1_init(void)
{
  CORE_foo.update_fn = demo1_update;
  CORE_foo.draw_fn = demo1_draw;
  CORE_foo.quit_fn = demo1_quit;

  SND_bgm_set("res/music/lose.wav");
  SND_bgm_play();

  CORE_window_icon(CORE_wnd, "res/favicon.ico");

  shader = GL_shader_create("res/shaders/htvs.glsl", "res/shaders/htfs.glsl");
  glUseProgram(shader);

  float vertices[] = {
    0.0f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
   -0.5f, -0.5f, 0.0f,
  };

  unsigned int VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  glBindVertexArray(0); 
}