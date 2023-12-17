#ifndef ANJIN_THREAD_H
#define ANJIN_THREAD_H

struct CORE_thread;

#ifdef _WIN32
  #define THREAD_FN     unsigned
  #define THREAD_RETURN return 0
#elif __EMSCRIPTEN__
  #define THREAD_FN     void *
  #define THREAD_RETURN return 0
#else
  #error Platform not supported (CORE::thread)
#endif

struct CORE_thread * CORE_thread_create (THREAD_FN (*fn)(void *));
void                 CORE_thread_destroy(struct CORE_thread *);
void                 CORE_thread_join   (struct CORE_thread *);
void                 CORE_sleep         (double time);

#endif