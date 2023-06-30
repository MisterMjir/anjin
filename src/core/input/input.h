#ifndef ANJIN_INPUT
#define ANJIN_INPUT

#include "keys.h"

struct CORE_input_mouse {
  int x;
  int y;
  int l;
  int r;
};

struct CORE_input {
  int quit;
  struct CORE_input_keys  keys;
  struct CORE_input_mouse mouse;
};

extern struct CORE_input CORE_input;

void CORE_input_loop  (void);
void CORE_input_update(void);

#endif