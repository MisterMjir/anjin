#include "gl_util.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/wgl.h>
#include "log/log.h"
#include "core/window/window.h"
#include <stdlib.h>
#include "gll.h"

/* This needs to match what is in window_win32.c */
struct CORE_window {
  HWND  h;
  HDC   dc;
  HGLRC rc;
};

PFNWGLCREATECONTEXTATTRIBSARBPROC cca;
PFNWGLCHOOSEPIXELFORMATARBPROC    cpf;
int GL_init(void)
{
  HWND                  w;
  HDC                   ctx;
  PIXELFORMATDESCRIPTOR pfd;
  int                   pfdid;
  HGLRC                 rc;

  w = CreateWindow("window_temp", "", WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
  ctx = GetDC(w);
  ZeroMemory(&pfd, sizeof(pfd));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cAlphaBits = 8;
  pfd.cDepthBits = 24;

  if (!(pfdid = ChoosePixelFormat(ctx, &pfd))) {
    LOG_msg("GL ChoosePixelFormat failed\n");
    return -1;
  }

  if (!SetPixelFormat(ctx, pfdid, &pfd)) {
    LOG_msg("GL SetPixelFormat failed\n");
    return -1;
  }

  if (!(rc = wglCreateContext(ctx))) {
    LOG_msg("GL wglCreateContext failed\n");
    return -1;
  }

  if (!wglMakeCurrent(ctx, rc)) {
    LOG_msg("GL wglMakeCurrent failed\n");
    return -1;
  }

  GL_gll();

  if (!(cpf = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB"))) {
    LOG_msg("GL wglGetProcAddress failed\n");
    return -1;
  }

  if (!(cca = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB"))) {
    LOG_msg("GL wglGetProcAddress failed\n");
    return -1;
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(rc);
  ReleaseDC(w, ctx);
  DestroyWindow(w);

  return 0;
}

int GL_window_init(struct CORE_window *w)
{
  const int attribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
    WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB,     32,
    WGL_ALPHA_BITS_ARB,     8,
    WGL_DEPTH_BITS_ARB,     24,
    WGL_STENCIL_BITS_ARB,   8,
    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    WGL_SAMPLES_ARB,        4,
    0
  };

  int                   pfid;
  UINT                  formats;
  int                   status;
  PIXELFORMATDESCRIPTOR pfd;

  if (!(status = cpf(w->dc, attribs, NULL, 1, &pfid, &formats)) || !formats) {
    LOG_msg("wglChoosePixelFormatARB failed");
    return -1;
  }

  DescribePixelFormat(w->dc, pfid, sizeof(pfd), &pfd);
  SetPixelFormat(w->dc, pfid, &pfd);

  return 0;
}

int GL_set(struct CORE_window *w)
{
  const int version_major = 3, version_minor = 3;
  int ctx_attribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, version_major,
    WGL_CONTEXT_MINOR_VERSION_ARB, version_minor,
    WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };

  if (!(w->rc = cca(w->dc, 0, ctx_attribs))) {
    LOG_msg("wglCreateContextAttribsARB failed\n");
    return -1;
  }

  if (!wglMakeCurrent(w->dc, w->rc)) {
    LOG_msg("wglMakeCurrent failed\n");
    return -1;
  }

  return 0;
}

void GL_unset(struct CORE_window *w)
{
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(w->rc);
}