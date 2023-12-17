void (*CORE_window_cb_size)(int, int);

#ifdef _WIN32
  #include "window_win32.c"
#elif __EMSCRIPTEN__
  #include "window_em.c"
#else
  #error Platform not supported (CORE::window)
#endif