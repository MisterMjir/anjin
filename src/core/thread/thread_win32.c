#include "thread.h"
#include <windows.h>
#include <process.h>

struct CORE_thread {
  HANDLE handle;
};

/**
 * CORE_thread_create
 *
 * Creates a thread and begins running it
 *
 * @param fn The thread function
 * @return   A pointer to a CORE_thread struct
 */
struct CORE_thread * CORE_thread_create(void (*fn)(void *))
{
  struct CORE_thread *t;

  if (!(t = malloc(sizeof(*t)))) {
    return NULL;
  }

  if (!(t->handle = (HANDLE) _beginthreadex(NULL, 0, fn, NULL, 0, NULL))) {
    return NULL;
  }

  return t;
}

/**
 * CORE_thread_destroy
 *
 * Destroys a thread
 *
 * @param t The thread to destroy
 */
void CORE_thread_destroy(struct CORE_thread *t)
{
  free(t);
}

/**
 * CORE_thread_join
 *
 * Waits for a thread to finish running
 *
 * @param t The thread to wait for
 */
void CORE_thread_join(struct CORE_thread *t)
{
  WaitForSingleObject(t->handle, INFINITE);
}

/**
 * CORE_sleep
 *
 * Waits for a specified amount of time
 *
 * @param t The wait time in milliseconds
 */
void CORE_sleep(double t)
{
  Sleep((DWORD) t);
}