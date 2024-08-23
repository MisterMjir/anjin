#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include "core/thread/thread.h"

void sound_init(void);
void sound_play(uint8_t);
THREAD_FN SND_loop_mafia(void* data);

#endif