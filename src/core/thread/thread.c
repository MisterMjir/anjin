#ifdef _WIN32
  #include "thread_win32.c"
#else
  #error Platform not supported (CORE::thread)
#endif