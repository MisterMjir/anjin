#include "snd.h"
#include <AL/alc.h>
#include "log/log.h"
#include "wav.h"
#include <stdlib.h>

#define BUFFERS_NUM 4
#define BUFFERS_SIZE 32768

struct SND_bgm {
  FILE         *file;
  ALuint        buffers[BUFFERS_NUM];
  ALenum        fmt;
  int           sample_rate;
  int           data_size;
  unsigned long data_start;
  ALuint        src;
  int           valid;
};

static ALCcontext    *ctx;
static ALCdevice     *device;
static volatile int   on;
static struct SND_bgm bgm;

/**
 * SND_init
 * 
 * Initializes everything needed for sound
 * 
 * @return Success
 */
int SND_init(void)
{
  bgm.valid = 0;
  if (!(device = alcOpenDevice(NULL))) {
    LOG_msg("Could not open an ALCdevice\n");
    return -1;
  }
  ctx = alcCreateContext(device, NULL);
  alcMakeContextCurrent(ctx);

  /* Initialize bgm to dummy values */
  return 0;
}

/**
 * SND_quit
 * 
 * Cleans up resources used by sound
 */
void SND_quit(void)
{
  on = 0;
  alcMakeContextCurrent(NULL);
  alcDestroyContext(ctx);
  alcCloseDevice(device);
}

/**
 * update_buffers
 * 
 * Loads however many new buffers need to be loaded
 * 
 * @return Success
 */
static int update_buffers(void)
{
  ALint         buffers_processed;
  ALuint        buffer;
  char         *data;
  unsigned long copy_size;
  unsigned long current_pos;

  alGetSourcei(bgm.src, AL_BUFFERS_PROCESSED, &buffers_processed);
  if (buffers_processed <= 0) return 1;

  while (buffers_processed--) {
    alSourceUnqueueBuffers(bgm.src, 1, &buffer);
    
    copy_size = BUFFERS_SIZE;
    if ((current_pos = ftell(bgm.file)) == -1) {
      LOG_msg("ftell failed\n");
      return -1;
    }

    if (current_pos + BUFFERS_SIZE > bgm.data_size)
      copy_size = bgm.data_size - current_pos;

    if (!(data = malloc(BUFFERS_SIZE))) {
      LOG_msg("Out of memory\n");
      return -1;
    }

    if (fread(data, 1, copy_size, bgm.file) != copy_size) {
      LOG_msg("Could not read wav file while updating buffers\n");
      return -1;
    }

    /* Leave no empty when looping back to the start of a song */
    if (copy_size < BUFFERS_SIZE) {
      if (fseek(bgm.file, bgm.data_start, SEEK_SET)) {
        LOG_msg("Could not seek wav file while updating buffers\n");
        return -1;
      }
      if (fread(&data[copy_size], 1, BUFFERS_SIZE - copy_size, bgm.file) != BUFFERS_SIZE - copy_size) {
        LOG_msg("Could not read wav while updating buffers\n");
        return -1;
      }
    }

    alBufferData(buffer, bgm.fmt, data, BUFFERS_SIZE, bgm.sample_rate);
    alSourceQueueBuffers(bgm.src, 1, &buffer);

    free(data);
  }

  return 0;
}

/**
 * SND_loop
 * 
 * The thread for sound
 * TODO: Add a sleep somehow
 * 
 * @param data Dummy parameter needed for thread functions
 */
THREAD_FN SND_loop(void *data)
{
  on = 1;
  while (on) {
    if (bgm.valid) {
      update_buffers();
    }
  }

  THREAD_RETURN;
}

/**
 * bgm_create
 * 
 * Sets up the bgm (background music)
 * 
 * @param fpath Path to the music file
 * @return      Success
 */
static int bgm_create(const char *fpath)
{
  struct SND_wav wav;
  char          *buffer;
  
  if (SND_wav_load(fpath, &wav)) {
    LOG_msg("Could not load wav file: %s\n", fpath);
    bgm.valid = 0;
    return -1;
  }

  bgm.sample_rate = wav.sr;
  bgm.data_size = wav.size;
  bgm.data_start = wav.ds;

  if (!(bgm.file = fopen(fpath, "rb"))) {
    LOG_msg("Could not load wav file: %s\n", fpath);
    bgm.valid = 0;
    return -1;
  }
  if (fseek(bgm.file, bgm.data_start, SEEK_SET)) {
    LOG_msg("Could not seek wav file");
    bgm.valid = 0;
    return -1;
  }

  alGenBuffers(BUFFERS_NUM, bgm.buffers);

  SND_wav_fmt(&wav, &bgm.fmt);

  /* Audio data must be bigger than BUFFERS_NUM * BUFFERS_SIZE */
  if (bgm.data_size < BUFFERS_NUM * BUFFERS_SIZE) {
    LOG_msg("Audio length is too small");
    bgm.valid = 0;
    return -1;
  }
  for (int i = 0; i < BUFFERS_NUM; ++i) {
    if (!(buffer = malloc(BUFFERS_SIZE))) {
      LOG_msg("Out of memory");
      bgm.valid = 0;
      return -1;
    }
    if (fread(buffer, 1, BUFFERS_SIZE, bgm.file) != BUFFERS_SIZE) {
      LOG_msg("Could not read wav file");
      bgm.valid = 0;
      return -1;
    }
    alBufferData(bgm.buffers[i], bgm.fmt, buffer, BUFFERS_SIZE, bgm.sample_rate);
    free(buffer);
  }

  alGenSources(1, &bgm.src);
  alSourcef(bgm.src, AL_PITCH, 1);
  alSourcef(bgm.src, AL_GAIN, 1.0f);
  alSource3f(bgm.src, AL_POSITION, 0, 0, 0);
  alSource3f(bgm.src, AL_VELOCITY, 0, 0, 0);
  alSourcei(bgm.src, AL_LOOPING, AL_FALSE);

  alSourceQueueBuffers(bgm.src, BUFFERS_NUM, bgm.buffers);

  bgm.valid = 1;

  return 0;
}

/**
 * bgm_destroy
 * 
 * Destroys the background music
 */
static void bgm_destroy(void)
{
  if (!bgm.valid) return;

  alDeleteSources(1, &bgm.src);

  alDeleteBuffers(BUFFERS_NUM, bgm.buffers);

  fclose(bgm.file);

  bgm.valid = 0;
}

/**
 * SND_bgm_set
 * 
 * Sets the background music, will stop playing the currently
 * playing one if there is one playing
 * 
 * @param fpath The music file
 */
int SND_bgm_set(const char *fpath)
{
  bgm_destroy();
  return bgm_create(fpath);
}

/**
 * SND_bgm_plays
 * 
 * Starts or resumes the background music
 */
void SND_bgm_play(void)
{
  alSourcePlay(bgm.src);
}

/**
 * SND_bgm_pause
 * 
 * Pauses the background music
 */
void SND_bgm_pause(void)
{
  alSourcePause(bgm.src);
}

/**
 * SND_bgm_state
 * 
 * The state of the background music (playing or not)
 */
int SND_bgm_state(void)
{
  ALenum s;
  alGetSourcei(bgm.src, AL_SOURCE_STATE, &s);

  return s == AL_PLAYING;
}