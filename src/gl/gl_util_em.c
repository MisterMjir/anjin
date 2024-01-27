#include "gl_util.h"
#include <emscripten/html5.h>

/* This needs to match what is in window_em.c */
struct CORE_window {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;
};

int GL_init(void)
{
  // Nothing needs to be done
  return 0;
}

int GL_window_init(struct CORE_window *w)
{
  // Nothing needs to be done
  return 0;
}

int GL_set(struct CORE_window *w)
{
  EmscriptenWebGLContextAttributes attribs;
  
  emscripten_webgl_init_context_attributes(&attribs);

  attribs.renderViaOffscreenBackBuffer = EM_TRUE;
  attribs.explicitSwapControl = EM_TRUE;
  attribs.premultipliedAlpha = EM_FALSE;
  attribs.majorVersion = 2;
  attribs.minorVersion = 0;

  w->gl_ctx = emscripten_webgl_create_context("#ANJIN_WINDOW", &attribs);

  emscripten_webgl_make_context_current(w->gl_ctx);

  return 0;
}

void GL_unset(struct CORE_window *w)
{
  emscripten_webgl_make_context_current(0);
  emscripten_webgl_destroy_context(w->gl_ctx);
}