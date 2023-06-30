void (*CORE_window_cb_size)(int, int);

#ifdef _WIN32
  #include "window_win32.c"
#else
  #error Platform not supported (CORE::window)
#endif