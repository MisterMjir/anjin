#ifndef ANJIN_SND_H
#define ANJIN_SND_H

#include <stdio.h>
#include "core/thread/thread.h"

int       SND_init     (void);
void      SND_quit     (void);
THREAD_FN SND_loop     (void *);
int       SND_bgm_set  (const char *);
void      SND_bgm_play (void);
void      SND_bgm_pause(void);
int       SND_bgm_state(void);

#endif