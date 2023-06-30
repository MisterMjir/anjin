#ifndef ANJIN_THREAD_H
#define ANJIN_THREAD_H

struct CORE_thread;

struct CORE_thread * CORE_thread_create (void (*fn)(void *));
void                 CORE_thread_destroy(struct CORE_thread *);
void                 CORE_thread_join   (struct CORE_thread *);
void                 CORE_sleep         (double time);

#endif