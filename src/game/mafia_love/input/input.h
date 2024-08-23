#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define INPUT_RTRG  0x1
#define INPUT_LTRG  0x2

struct input {
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
  uint8_t buttons;
	uint8_t pause;
};

void         input_init        (void);
struct input input_get         (void);
uint8_t      input_paused      (void);
void         input_clear_pause (void);
void         input_clear_button(uint8_t flag);

#endif