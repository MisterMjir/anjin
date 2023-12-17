#ifdef _WIN32
  #include "gl_util_win32.c"
#elif __EMSCRIPTEN__
  #include "gl_util_em.c"
#else
  #error Platform not supported (GL)
#endif