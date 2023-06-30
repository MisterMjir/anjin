#ifndef ANJIN_WINDOW_H
#define ANJIN_WINDOW_H

struct CORE_window;

int                  CORE_window_init        (void);
struct CORE_window * CORE_window_create      (int w, int h);
void                 CORE_window_destroy     (struct CORE_window *);
void                 CORE_window_swap_buffers(struct CORE_window *);
void                 CORE_window_fullscreen  (struct CORE_window *, int);
void                 CORE_window_icon        (struct CORE_window *, const char *);
void                 CORE_window_title       (struct CORE_window *, const char *);

/* Callbacks */
extern void (*CORE_window_cb_size)(int, int);

#endif