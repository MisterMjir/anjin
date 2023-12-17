#ifdef _WIN32
  #include "thread_win32.c"
#elif __EMSCRIPTEN__
  #include "thread_em.c"
#else
  #error Platform not supported (CORE::thread)
#endif