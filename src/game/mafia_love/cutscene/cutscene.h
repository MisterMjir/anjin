#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <stdint.h>

#define CUTSCENE_LANGUAGES 2

#define CUTSCENE_PROLOGUE 0
#define CUTSCENE_1        1
#define CUTSCENE_2        2
#define CUTSCENE_3        3

struct dialogue {
	uint8_t     img;
	uint8_t     ix;
	uint8_t     iy;
	uint8_t     tx;
	uint8_t     ty;
	uint8_t     sound; /* Make 0xEF to skip */
	const char *name;
	const char *text[CUTSCENE_LANGUAGES];
};

void cutscene_init(uint8_t cutscene);

#endif