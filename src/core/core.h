#ifndef ANJIN_CORE_H
#define ANJIN_CORE_H

#include "input/input.h"
#include "thread/thread.h"
#include "window/window.h"

struct CORE_foo {
  int    fps;              /* Frames per second */
  double td;               /* Tick delay */
  void (*init_fn)  (void); /* State init function */
  void (*quit_fn)  (void); /* State quit function */
  void (*update_fn)(void); /* State update function */
  void (*draw_fn)  (void); /* State draw function */
  int    init;             /* Init a new state */
  int    win_w;            /* New window width */
  int    win_h;            /* New window height */
  int    resize;           /* Resize window */
};

extern struct CORE_foo     CORE_foo;
extern struct CORE_window *CORE_wnd;

int  CORE_init        (int w, int h, unsigned int fps, void (*init_fn)(void));
void CORE_quit        (void);
void CORE_game_loop   (void *);
void CORE_set_fps     (unsigned int);
void CORE_switch_state(void (*init_fn)(void));

#endif