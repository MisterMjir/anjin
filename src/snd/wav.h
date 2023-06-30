#ifndef ANJIN_WAV_H
#define ANJIN_WAV_H

#include <stdint.h>
#include <AL/al.h>

struct SND_wav {
  uint8_t  cn;   /* Channels */
  uint32_t sr;   /* Sample rate */
  uint8_t  bps;  /* Bits per sample */
  int32_t  size; /* Total size */
  int32_t  ds;   /* Data start */
};

int SND_wav_load(const char *fpath, struct SND_wav *snd);
int SND_wav_fmt (struct SND_wav *wav, ALenum *fmt);

#endif