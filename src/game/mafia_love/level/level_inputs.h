#ifndef LEVEL_INPUTS_H
#define LEVEL_INPUTS_H

#include <stdint.h>
#include "../input/input.h"

void level_inputs(struct input *input, uint16_t *x, uint16_t *y, uint16_t *phi, const uint8_t *world, uint16_t worldw);

#endif