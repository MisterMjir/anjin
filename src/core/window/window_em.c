#include "window.h"
#include <emscripten/html5.h>
#include <stdio.h>

struct CORE_window {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;
};

int CORE_window_init(void)
{
  // Nothing needs to be done
  return 0;
}

struct CORE_window * CORE_window_create(int w, int h)
{
  struct CORE_window *win;

  EM_ASM(
    let loading = document.getElementById("loading");
    document.body.removeChild(loading);
    let window = document.createElement('canvas');
    window.id = 'ANJIN_WINDOW';
    window.style.backgroundColor = '#000';
    document.body.appendChild(window);
  );

  if (!(win = malloc(sizeof(*win)))) {
    fprintf(stderr, "Out of memory\n");
    return NULL;
  }

  emscripten_set_canvas_element_size("#ANJIN_WINDOW", w, h);

  return win;
}

void CORE_window_destroy(struct CORE_window *w)
{
  free(w);
}

void CORE_window_swap_buffers(struct CORE_window *w)
{
  emscripten_webgl_commit_frame();
}

void CORE_window_fullscreen(struct CORE_window *w, int fs)
{
  // TODO
}

void CORE_window_icon(struct CORE_window *w, const char *fpath)
{
  // TODO
}

void CORE_window_title(struct CORE_window *w, const char *str)
{
  EM_ASM({document.title = UTF8ToString($0);}, str);
}