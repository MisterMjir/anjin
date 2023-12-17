#ifdef _WIN32
  #include "gll_win32.c"
#elif __EMSCRIPTEN__
  #include "gll_em.c"
#else
  #error Platform not supported (GL)
#endif