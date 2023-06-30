#include "wav.h"
#include "log/log.h"
#include <stdio.h>
#include <string.h>

#define READ(count) if (fread(temp, sizeof(*temp), count, file) != count) {LOG_msg("Could not read in wav file\n"); return -1;}
#define CHECK(text, count) strncmp(temp, text, count)
#define CHECK_ERR(text, count) if (strncmp(temp, text, count)) {LOG_msg("Poorly formmated wav file\n"); return -1;}
#define SEEK(var, loc) if (fseek(file, var, loc)) {LOG_msg("Could not seek in wav file\n"); return -1;}
int SND_wav_load(const char *fpath, struct SND_wav *s)
{
  FILE    *file;
  char     temp[4];
  uint32_t fmt_end;
  uint32_t offset;

  if (!(file = fopen(fpath, "rb"))) return -1;

  /* Read the RIFF */
  READ(4); /* Chunk id */
  READ(4); /* Chunk size */
  READ(4); /* File format */
  CHECK_ERR("WAVE", 4);

  /* Read fmt (some files have JUNK after RIFF) */
  READ(4); /* Chunk id */
  while (CHECK("fmt", 3)) {
    READ(4); /* Chunk size */
    uint32_t offset = *(uint32_t *) temp;
    SEEK(offset, SEEK_CUR); /* Skip past the chunk */
    READ(4); /* Chunk id */
  }
  READ(4); /* Chunk size */
  fmt_end = ftell(file) + *(uint32_t *) temp;
  READ(2); /* PCM */
  READ(2); /* Channel num */
  s->cn = *(uint8_t *) temp;
  READ(4); /* Sample rate */
  s->sr = *(int32_t *) temp;
  READ(4); /* Byte rate */
  READ(2); /* Block align */
  READ(2); /* Bits per sample */
  s->bps = *(uint8_t *) temp;

  if (ftell(file) != fmt_end)
    SEEK(fmt_end, SEEK_SET);

  /* Data */
  READ(4); /* Chunk id */
  if (CHECK("data", 4)) {
    while (CHECK("data", 4)) {
      READ(4); /* Chunk size */
      offset = *(uint32_t *) temp;
      SEEK(offset, SEEK_CUR); /* Skip past the chunk */
      READ(4); /* Chunk id */
    }
  }
  READ(4); /* Chunk size */
  s->size = *(int32_t *) temp;

  s->ds = ftell(file);

  fclose(file);

  return 0;
}

#define FMT_CHECK(cnp, bpsp, fmtp) if (wav->cn == cnp && wav->bps == bpsp) {*fmt = fmtp; return 0;}
int SND_wav_fmt(struct SND_wav *wav, ALenum *fmt)
{
  FMT_CHECK(1, 8,  AL_FORMAT_MONO8);
  FMT_CHECK(1, 16, AL_FORMAT_MONO16);
  FMT_CHECK(2, 8,  AL_FORMAT_STEREO8);
  FMT_CHECK(2, 16, AL_FORMAT_STEREO16);

  LOG_msg("Wav has unrecognized audio format\n");
  return -1;
}