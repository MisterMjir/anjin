#include "thread.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>

struct CORE_thread {
  pthread_t pthread;
};

struct CORE_thread * CORE_thread_create(THREAD_FN (*fn)(void *))
{
  struct CORE_thread *t;

  if (!(t = malloc(sizeof(*t)))) {
    fprintf(stderr, "Out of memory");
    return NULL;
  }

  pthread_create(&t->pthread, NULL, fn, NULL);

  return t;
}

void CORE_thread_destroy(struct CORE_thread *t)
{
  free(t);
}

void CORE_thread_join(struct CORE_thread *t)
{
  pthread_join(t->pthread, NULL);
}

void CORE_sleep(double time)
{
  emscripten_sleep(time * 1000);
}