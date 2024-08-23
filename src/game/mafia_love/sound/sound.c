#include "sound.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/core.h"

struct music {
	uint32_t sector_start;
	uint32_t length;
};

const struct music music_table[] = {
	{    0, 3276800},
	{ 6400, 1872000},
	{10057, 1653760},
	{13287, 2544614},
	{18257, 3420588},
	{24938,  619203},
	{26148, 3168828},
	{32338,  874510}
};

// port note
/*
 * This is the hardest thing to port, a separate thread is even made
 * to mimic the irl functionality. It's probably implemented pretty bad
 */

#define FMT AL_FORMAT_MONO8
#define SAMPLE_RATE 12000

#define BUFFERS_NUM 4
#define BUFFER_SIZE 512

static FILE *file;

static ALuint buffers[BUFFERS_NUM];
static uint8_t current_buffer = 0;

static ALCcontext *ctx;
static ALCdevice *device;
static ALuint src;
static int has_init;

static int length;
static int current_song;

/*
 * init
 *
 * @desc
 *   Initialize sound (UART)
 */
void sound_init(void)
{
	if (!(device = alcOpenDevice(NULL))) {
		return;
	}
	ctx = alcCreateContext(device, NULL);
	alcMakeContextCurrent(ctx);

	file = fopen("res/mafia_love/songs.img", "rb");
}

static int update_buffers(void)
{
	ALint buffers_processed;
	ALuint buffer;
	unsigned long copy_size;
	uint8_t *data;
	unsigned long current_pos;
	
	alGetSourcei(src, AL_BUFFERS_PROCESSED, &buffers_processed);
	if (buffers_processed <= 0) return 1;

	while (buffers_processed--) {
		alSourceUnqueueBuffers(src, 1, &buffer);

		copy_size = BUFFER_SIZE;

		current_pos = ftell(file);
		if (current_pos + BUFFER_SIZE > music_table[current_song].sector_start * 512 + length)
			copy_size = music_table[current_song].sector_start * 512 + length - current_pos;

		data = malloc(BUFFER_SIZE);
		fread(data, 1, copy_size, file);

		if (copy_size < BUFFER_SIZE) {
			fseek(file, music_table[current_song].sector_start * 512, SEEK_SET);
			fread(&data[copy_size], 1, BUFFER_SIZE - copy_size, file);
		}

		// Convert data into MONO8
		// 6 bit dac values, honestly will try without doing anything

		alBufferData(buffer, FMT, data, BUFFER_SIZE, SAMPLE_RATE);
		alSourceQueueBuffers(src, 1, &buffer);

		free(data);
	}

	return 0;
}

THREAD_FN SND_loop_mafia(void *data)
{
	while (!CORE_input.quit) {
		if (has_init) {
			update_buffers();
		}
	}

	THREAD_RETURN;
}

/*
 * play
 *
 * @desc
 *   Play sfx or music
 */
void sound_play(uint8_t command)
{
	if (command == 0xFF) return; // stops music
	if (command & 0x80) return; // sfx

	has_init = 0;

	// Delete
	alDeleteSources(1, &src);
	alDeleteBuffers(BUFFERS_NUM, buffers);

	// Create
	uint8_t *buffer;
	current_song = command;
	length = music_table[current_song].length;
	fseek(file, music_table[current_song].sector_start * 512, SEEK_SET);

	alGenBuffers(BUFFERS_NUM, buffers);

	for (int i = 0; i < BUFFERS_NUM; ++i) {
		buffer = malloc(BUFFER_SIZE);
		fread(buffer, 1, BUFFER_SIZE, file);
		
		alBufferData(buffers[i], FMT, buffer, BUFFER_SIZE, SAMPLE_RATE);
		free(buffer);
	}

	alGenSources(1, &src);
	alSourcef(src, AL_PITCH, 1);
	alSourcef(src, AL_GAIN, 1.0f);
	alSource3f(src, AL_POSITION, 0, 0, 0);
	alSource3f(src, AL_VELOCITY, 0, 0, 0);
	alSourcei(src, AL_LOOPING, AL_FALSE);

	alSourceQueueBuffers(src, BUFFERS_NUM, buffers);
	alSourcePlay(src);

	has_init = 1;
}
